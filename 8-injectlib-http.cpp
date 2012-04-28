#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>

#include <microhttpd.h>

//#define DEBUG

#include "common.h"
#include "clothes_controller.cpp"
#include "items_controller.cpp"
#include "units_controller.cpp"
#include "trade_controller.cpp"

static const char* ALLOWED_CONTENT_TYPES[][2] = {
    {".js",  "application/x-javascript"},
    {".css", "text/css"},
    {".gif", "image/gif"},
    {".png", "image/png"}
};

static pid_t(*orig_getpid)() = NULL;

static struct MHD_Daemon* mhd = NULL;

void dostuff(int);
void memserver_start();
void memserver_accept();

pid_t getpid(){
    if(!mhd){
        orig_getpid = (pid_t (*)()) dlsym(RTLD_NEXT, "getpid");
        printf("[.] (pid=%d) starting server...\n",orig_getpid());
        memserver_start();
    }
    memserver_accept();
    return orig_getpid();
}

///////////////////////////////////////////////////////////////////////////////////

struct hexdump_params{
    unsigned long offset;
    unsigned long size;
    int width;
};

int get_hexdump_params(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    struct hexdump_params* hp = (struct hexdump_params*)cls;
    if(!strcmp(key, "offset")){
        hp->offset = strtoul(value, NULL, 0);
    } else if(!strcmp(key, "size")){
        hp->size = strtoul(value, NULL, 0);
    } else if(!strcmp(key, "width")){
        hp->width= atoi(value);
    }
    return MHD_YES;
}

///////////////////////////////////////////////////////////////////////////////////

static int ahc_echo(void * cls,
                    struct MHD_Connection * conn,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr) {
  static int dummy;
  struct MHD_Response * response;
  int ret;
  char buf[0x1000];
  string html,s;
  int is_json = 0;
  int resp_code = MHD_HTTP_OK;

  HTTPRequest request(conn, url);

  if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */

  if (&dummy != *ptr){
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }

  if (0 != *upload_data_size) return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL; /* clear context pointer */

  html.reserve(50*1024);

  if(request.url_match("/dwarves") || request.url_match("/units")){
      UnitsController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(request.url_match("/clothes")){
      ClothesController c(request);
      html.reserve(100*1024);
      html += c.to_html();

  } else if(!strcmp(url, "/items")){
      ItemsController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(!strcmp(url, "/trade")){
      TradeController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(!strcmp(url,"/hexdump")){
      struct hexdump_params hp; hp.size = hp.offset = 0; hp.width = 1;
      MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &get_hexdump_params, &hp);

      string title = request.get_string("title","");
      if(!title.empty()){
          html += "<title>" + html_escape(title) + "</title>\n";
          html += "<h2>" + html_escape(title) + "</h2>\n";
      }

      html += "<pre>";
      switch(hp.width){
          case 4:{
                  uint32_t *p = (uint32_t*)hp.offset;
                  for(long i=0; i<hp.size; i+=4, p++){
                      if(i%0x10 == 0){ sprintf(buf, "\n%08lx: ", i); html += buf; }
                      sprintf(buf, " %08x", *p); 
                      html += buf;
                  }
                 }
              break;
          default:{
                  unsigned char*p = (unsigned char*)hp.offset;
                  for(long i=0; i<hp.size; i++, p++){
                      if(i%0x10 == 0){ sprintf(buf, "\n%08lx: ", i); html += buf; }
                      sprintf(buf, " %02x", *p); 
                      html += buf;
                  }
                  }
              break;
      }
      html += "</pre>";
  } else {
      // security
      if(strstr(url, "..") || strstr(url,"//") || strchr(url,'\\')) return MHD_NO;

      int lUrl = strlen(url);
      for(int i=0; i<sizeof(ALLOWED_CONTENT_TYPES)/sizeof(ALLOWED_CONTENT_TYPES[0]); i++){
          const char* ext   = ALLOWED_CONTENT_TYPES[i][0];
          const char* ctype = ALLOWED_CONTENT_TYPES[i][1];
          int lExt = strlen(ext);

          if(lUrl > lExt && !strcmp(url+lUrl-lExt,ext)){
              sprintf(buf,"dwarf_keeper%s",url);
              int fd;
              struct stat sbuf;
              if ( (-1 == (fd = open (buf, O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
                  sprintf(buf, "[!] %s: ", url);
                  perror(buf);
                  if (fd != -1) close (fd);
                  // file not found or cannot open
                  return MHD_NO;
              }
              response = MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
              MHD_add_response_header (response, "Content-Type", ctype);
              ret = MHD_queue_response (conn, MHD_HTTP_OK, response);
              MHD_destroy_response (response);
              return ret;
          }
      }

      // not allowed file extension
      return MHD_NO;
  }
      

  string html_utf8;
  if(is_json){
      html_utf8 = html;
  } else if(request.is_ajax()){
      html_utf8 = cp850_to_utf8(html);
  } else {
      int fd;
      struct stat sbuf;
      size_t size = 0;

      if ( (-1 == (fd = open ("dwarf_keeper/template.html", O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
          perror("[!] template.html: ");
          if (fd != -1) close (fd);
      } else {
          html_utf8.resize(sbuf.st_size); // fills with NULL chars by default
          read(fd, &html_utf8[0], sbuf.st_size);
          close(fd);
      }

      html = cp850_to_utf8(html);

      if( str_replace(html_utf8, "{CONTENT}", html) == 0 ){
          html_utf8 =
              "<html><head>\n"
              "\t<link href='/style.css' rel='stylesheet' type='text/css' />\n"
              "</head><body>\n" +
              html + "\n\n" +
              "<script src='/jquery-1.7.2.min.js'></script>\n"
              "<script src='/sorttable.js'></script>\n"
              "<script src='/dwarf_keeper.js'></script>\n"
              "<font color=red>"
                  " no {CONTENT} tag in template.html, or template.html not found. Using built-in default"
              "</font>\n"
              "</body></html>";
      }
  }

  response = MHD_create_response_from_buffer(html_utf8.size(), (void*) html_utf8.data(), MHD_RESPMEM_MUST_COPY);
  if(is_json){
      MHD_add_response_header(response, "Content-Type", "application/json");
  } else {
      MHD_add_response_header(response, "Content-Type", "text/html; charset=utf-8");
  }
  ret = MHD_queue_response(conn, resp_code, response);
  MHD_destroy_response(response);

  return ret;
}

void memserver_start(){
  mhd = MHD_start_daemon(0,
                       4545,
                       NULL, NULL,      // accept policy  callback + argument
                       &ahc_echo, NULL, // access handler callback + argument
                       MHD_OPTION_END);
  if (mhd == NULL){
    perror("[!] failed to start HTTP server: ");
    exit(1);
  }
}

void memserver_accept(){
      fd_set sr,sw,sx;
      int max_fd = 0;
      struct timeval tv;
      unsigned MHD_LONG_LONG mhd_timeout;

      FD_ZERO(&sr); FD_ZERO(&sw); FD_ZERO(&sx);

      if( MHD_YES != MHD_get_fdset(mhd,&sr,&sw,&sx,&max_fd)){
          perror("[!] MHD_fet_fdset failed: ");
          exit(1);
      }

      tv.tv_sec = 0;
      tv.tv_usec = 1000;

      select (max_fd + 1, &sr, &sw, &sx, &tv);
      MHD_run(mhd);
}
