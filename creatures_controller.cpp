#include "common.h"

class CreaturesController {
    HTTPRequest* request;

    public:

    int resp_code;

    CreaturesController(HTTPRequest& req){
        request = &req;
        resp_code = MHD_HTTP_OK;
    }

    string to_html(){
        int id = request->get_int("id",0);
        if(id){
            return show(id);
        } else {
            return list();
        }
    }

    // show one creature
    string show(int id){
        int idx = 0;
        char buf[0x200];
        string html;
        bool found = false;
        
        while(Creature* pc=Creature::getNext(&idx)){
            if(id == pc->getId()){
                found = true;
                html += "<div id=dwarf>\n";
                html += "<h1>" + pc->getName() + "</h1>\n";

                sprintf(buf, "<div id=happiness>%d</div>\n", pc->getHappiness()); html += buf;

                // wearings

                WearingVector*wv = pc->getWear();
                if(wv && wv->size() > 0){
                    html += "<table class='items sortable'>\n";
                    html += "<tr><th>item <th class=sorttable_numeric>value\n";

                    WearingVector::iterator itr;
                    for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                        html += HTML::Item((*itr)->item);
                    }

                    html += "</table>\n";
                }

                // skills

                SkillsVector* psv = pc->getSoul()->getSkillsVector();
                if(psv && psv->size() > 0){
                    html += "<table id=skills class='sortable skills'>\n";
                    html += "<tr><th class=sorttable_numeric>level<th>skill\n";
                    for(SkillsVector::iterator it=psv->begin(); it<psv->end(); it++){
                        const char*type = NULL;
                        switch((*it)->getType()){
                            case Skill::TYPE_COMBAT:
                                type = "st_combat";
                                break;
                            case Skill::TYPE_LABOR:
                                type = "st_labor";
                                break;
                            case Skill::TYPE_MISC:
                                type = "st_misc";
                                break;
                        }
                        sprintf(buf, "<tr class='%s'><td class=skill_level><tt>%2d</tt> %s <td class=skill_name>%s \n",
                                type ? type : "",
                                (*it)->getLevel(),
                                (*it)->levelString().c_str(),
                                (*it)->nameString(pc->getRace(), pc->getSex()).c_str()
                                ); html+=buf;
                    }
                    html += "</table>\n";
                }


                html += "<div class=thoughts>" + pc->getThoughts() + "</div>\n";
                html += "</div>\n"; // div id=dwarf

                break;
            }
        }
        if(!found){
            html += "Not Found";
            resp_code = MHD_HTTP_NOT_FOUND;
        }

        return html;
    }

    // list of creatures
    string list(){
        string html,s;
        char buf[0x1000];

        html += "<table class='dwarves sortable'>";
        html += "<tr>"
            "<th>name "
            "<th>profession "
            "<th title='number of items weared'>items "
            "<th class=sorttable_numeric title='total items value'>value "
            "<th class=sorttable_numeric title='greater is better'>happiness"
            "\n";
        html += "<th class=flags>flags";
        html += "<th>test";

        int idx = 0, nDwarves = 0, race_filter;

        if( request->url_match("/dwarves") ){
            race_filter = RACE_DWARF;
        } else {
            race_filter = request->get_int("race",-1);
        }

        while(Creature* pc=Creature::getNext(&idx, race_filter)){
            s = pc->getName();

            nDwarves++;

            // dwarven babies have no useful info
            if(s.find(", Dwarven Baby") != string::npos) continue;

            if(!str_replace(s, ", ", "</a></td><td class=prof>")){
                // no profession
                s += "</a></td><td class=prof>";
            }

            sprintf(buf, "<tr id=\"dwarf_%04x\"><td><a href=\"?id=%d\">%s</td>", 
                    pc->getId(),
                    pc->getId(),
                    s.c_str());
            html += buf;

            int nItems = 0, totalValue = 0;
            WearingVector*wv = pc->getWear();
            WearingVector::iterator itr;
            for ( itr = wv->begin(); itr < wv->end(); ++itr ) {
                Item* pItem = (*itr)->item;
                totalValue += pItem->getValue();
                nItems++;
            }
            sprintf(buf, "<td class=r>%d</td><td class=r>%d<span class=currency>&#9788;</span></td>", nItems, totalValue);
            html += buf;

            sprintf(buf, "<td class=r>%d</td>", pc->getHappiness());
            html += buf;

            sprintf(buf, "<td class='flags r'>%x</td>", pc->getFlags());
            html += buf;

            sprintf(buf, "<td class='flags r'>%x</td>", pc->dw(0x5fc) - pc->dw(0x5f8));
            html += buf;

            html += "</tr>\n";
        }
        html += "</table>";

        sprintf(buf, "<h1>%s (%d)</h1>\n", race_filter == RACE_DWARF ? "Dwarves" : "Creatures" ,nDwarves);
        html = buf + html;

        return html;
    }
};