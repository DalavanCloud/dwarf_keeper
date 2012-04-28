#include "controller.cpp"

static const char* GEM_CUTS[] = { 
    "square", "oval", "tapered", "octagon", "rectangular", "round", "cushion", "emerald",
    "table", "baguette", "brilliant", "radiant", "marquise", "briolette", "trillion",
    "pear", "point", "cushion", "single"
};

static const char* TREE_NAMES[] = {
    "black-cap", "blood thorn", "tower-cap", "alder", "spore tree", "ashen", "tunnel tube",
    "cedar", "fungiwood"
};

class ItemsController : Controller {
    map<uint32_t, int> counts_map;
    HTTPRequest* request;
    int want_group;

    public:
    int resp_code;

    ItemsController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
        want_group = req.get_int("group", 1);
    }

    string to_html(){
        int type_id, id;
        string html;

        if( -1 != (id = request->get_int("id", -1))){
            if(Item* item = Item::find(id)){
                return show(item);
            } else {
                resp_code = MHD_HTTP_NOT_FOUND;
                return "Not Found";
            }
        }

        html += "<div id=items>\n";
        html += index();

        type_id = request->get_int("t", -1);
        if(type_id != -1){
            if(want_group)
                html += grouped_items(type_id);
            else
                html += ungrouped_items(type_id);
        }

        html += "</div>\n";
        return html;
    }

    private:

    string show(Item *item){
        string html;
        char buf[0x200];

        html += "<div id=item>\n";
        html += "<h1>" + item->getName() + "</h1>\n";

        html += "<table class=t1>\n";

        html += "<tr><th> full name <td class=fullname>" + item->getName() + "\n";
        html += "<tr><th> base name <td class=basename>" + item->getBaseName(0) + "\n";

        sprintf(buf, "<tr><th>value <td class=r>%d<span class=currency>&#9788;</span>\n", 
                item->getValue()); html += buf;

        sprintf(buf, "<tr><th>flags <td class='r comment'>%x\n", 
                item->getFlags()); html += buf;

        sprintf(buf, "<tr><th>ptr <td class='r ptr'><a href='/hexdump?offset=%p&size=%d&width=4'>%p</a>\n", 
                item, Item::RECORD_SIZE, item
                ); html += buf;

        html += "</table>\n";

        RefsVector* refs = item->getRefs();
        if( refs && refs->size() > 0 ){
            html += "<table id=refs class=t1>\n";
            for(int i=0; i<refs->size(); i++){
                Reference *ref = refs->at(i);

                html += "<tr><th>";
                html += ref->getTypeString();

                if(Unit *p = ref->getUnit()){
                    sprintf(buf, "<td><a href='/units?id=%d'>%s</a>", p->getId(), p->getName().c_str());
                    html += buf;
                }

                if(Item *p = ref->getItem()){
                    html += "<td>";
                    html += link_to_item(p);
                }

                if(Building *p = ref->getBuilding()){
                    html += "<td>" + html_escape(p->getName());
                }

                string s = ref->getDescription();
                if( !s.empty() ) html += "<td>" + html_escape(s);
            }
            html += "</table>\n";
        }

        html += "</div>\n";

        return html;
    }

    struct count_chunk { int count; int total_value; Item*pItem; };

    string grouped_items(int type_id){
        string html, name;
        ItemsVector*v = Item::getVector();
        char buf[0x200];
        int cnt = 0, pos, add_count;
        map <string, count_chunk> m;

        html += "<table class='items sortable'>\n"
            "<tr>"
            "<th>item"
            "<th class=sorttable_numeric title='average value'>value"
            "<th>count"
            "<th class=sorttable_numeric title='total value'>total"
            "\n";

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if(type_id != (*itr)->getTypeId()) continue;

            add_count = 1; // one item by default

            switch(type_id){
                case ItemType::SMALL_CUT_GEMS:
                    name = (*itr)->getName();
                    pos = name.find(" cut ");
                    if( pos != string::npos ) name.erase(0,pos+1);

                    // "oval prases" => "cut prases"
                    for(int i=0; i<sizeof(GEM_CUTS)/sizeof(GEM_CUTS[0]); i++){
                        if( name.substr(0, strlen(GEM_CUTS[i])) == GEM_CUTS[i] ){
                            name.replace(0,strlen(GEM_CUTS[i]), "cut");
                            break;
                        }
                    }   

                    // "cut alexandrite cabochons" => "cut alexandrites"
                    if( name.size() > 10 && name.substr(name.size()-10,10) == " cabochons" ){
                        name.replace(name.size()-10, 10, "s");
                    }

                    // "cut golden beryls)" => "cut golden beryls"
                    if( name[0] != '(' && name[name.size()-1] == ')'){
                        name.erase(name.size()-1,1);
                    }
                    break;

                case ItemType::AMMUNITION:
                    // ignore quality modifiers
                    name = (*itr)->getBaseName(0);

                    // "walrus bone bolts [5]" => "bone bolts [5]"
                    pos = name.find(" bone bolt");
                    if(pos != string::npos) name.erase(0,pos+1);

                    // "bone bolts [5]" => "bone bolts" + increase counter by 5
                    pos = name.find(" [");
                    if(pos != string::npos && name.find("]") == name.size()-1){
                        add_count = atoi(name.substr(pos+2, name.size()-pos-3).c_str());
                        name.erase(pos);
                    }

                    // "bone bolt"  => "bone bolts"
                    // "ashen dart" => "ashen darts"
                    if( name.size() > 0 && name[name.size()-1] != 's'){
                        name += "s";
                    }

                    // "ashen bolts" => "wood bolts"
                    for(int i=0; i<sizeof(TREE_NAMES)/sizeof(TREE_NAMES[0]); i++){
                        if( name.substr(0, strlen(TREE_NAMES[i])) == TREE_NAMES[i] ){
                            name.replace(0,strlen(TREE_NAMES[i]), "wood");
                            break;
                        }
                    }   
                    break;

                case ItemType::CLOTH:
                    // ignore quality modifiers
                    name = (*itr)->getBaseName(0);
                    break;

                default:
                    name = (*itr)->getName();
                    break;
            }
            count_chunk& cc = m[name];
            cc.count += add_count;
            cc.total_value += (*itr)->getValue();
            cc.pItem = *itr;
            cnt++;
        }

        for(map<string,count_chunk>::iterator it = m.begin(); it != m.end(); it++){
            const char* title = it->first.c_str();

            // if there's only one item of a kind => show it with link to actual item info page
            if(it->second.count == 1) title = link_to_item(it->second.pItem, title);
            html += HTML::Item(title, it->second.total_value / it->second.count);

            sprintf(buf, "<td class=r>%d <td class=r>%d<span class=currency>&#9788;</span> <td class=comment>%s\n",
                    it->second.count,
                    it->second.total_value,
                    it->second.pItem->getBaseName(0).c_str()
                    );
            html += buf;
        }

        html += "</table>\n";

        sprintf(buf, "<h2>%d items of type %d</h2>\n", cnt, type_id);
        html = buf + html;

        return html;
    }

    string ungrouped_items(int type_id){
        string html, name;
        ItemsVector*v = Item::getVector();
        char buf[0x200];
        int cnt = 0, pos, add_count;
        map <string, count_chunk> m;

        html += "<table class='items sortable'>\n"
            "<tr>"
            "<th>item"
            "<th class=sorttable_numeric title='value'>value"
            "\n";

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            if(type_id != (*itr)->getTypeId()) continue;

            html += HTML::Item(link_to_item(*itr), (*itr)->getValue());

#ifdef DEBUG
            sprintf(buf,
                  "<td class=r>"
                  "<a class=ptr href='/hexdump?offset=%p&size=%d&width=4&title=%s'>"
                  "%p"
                  "</a>",
                  *itr, Item::RECORD_SIZE, url_escape((*itr)->getName()).c_str(), *itr);
            html += buf;
#endif

            cnt++;
        }

        html += "</table>\n";

        sprintf(buf, "<h2>%d items of type %d</h2>\n", cnt, type_id);
        html = buf + html;

        return html;
    }

    string index(){
        string html;
        char buf[0x200];

        BENCH_START;
        count_by_types();
        BENCH_END("ItemsController::count_by_types");

        html += "<table class='counts-by-type sortable'>\n";
        html += "<tr> <th>type <th>count\n";

        for(map<uint32_t,int>::iterator it = counts_map.begin(); it != counts_map.end(); it++){
            sprintf(buf, "<tr><td>%s <td class=r><a href='?t=%d'>%d</a>\n", 
                    ItemType::type2string(it->first), 
                    it->first, 
                    it->second);
            html += buf;
        }
        html += "</table>\n";

        return html;
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    void count_by_types(){
        ItemsVector*v = Item::getVector();

        for( ItemsVector::iterator itr = v->begin(); itr < v->end(); ++itr) {
            counts_map[(*itr)->getTypeId()]++;
        }
    }
};
