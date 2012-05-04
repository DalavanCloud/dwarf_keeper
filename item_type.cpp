class ItemType {
    public:

    static const int BARS                   =   0;
    static const int SMALL_CUT_GEMS         =   1;
    static const int BLOCKS                 =   2;
    static const int ROUGH_GEMS             =   3;
    static const int STONES                 =   4;
    static const int LOGS                   =   5;
    static const int DOORS                  =   6;
    static const int FLOODGATES             =   7;
    static const int BEDS                   =   8;
    static const int THRONES                =   9;
    static const int CHAINS_ROPES           =  10;
    static const int FLASKS                 =  11;
    static const int GOBLETS                =  12;
    static const int MUSICAL_INSTRUMENTS    =  13;
    static const int TOYS                   =  14;
    static const int CAGES                  =  16;
    static const int BARRELS                =  17;
    static const int BUCKETS                =  18;
    static const int TABLES                 =  20;
    static const int COFFINS                =  21;
    static const int STATUES                =  22;
    static const int CORPSES                =  23;
    static const int WEAPONS                =  24;
    static const int BODYWEAR               =  25;
    static const int FOOTWEAR               =  26;
    static const int SHIELDS                =  27;
    static const int HEADWEAR               =  28;
    static const int HANDWEAR               =  29;
    static const int BOXES_BAGS             =  30;
    static const int BINS                   =  31;
    static const int ARMOR_STANDS           =  32;
    static const int WEAPON_RACKS           =  33;
    static const int CABINETS               =  34;
    static const int FIGURINES              =  35;
    static const int AMULETS                =  36;
    static const int SCEPTERS               =  37;
    static const int AMMUNITION             =  38;
    static const int CROWNS                 =  39;
    static const int RINGS                  =  40;
    static const int EARRINGS               =  41;
    static const int BRACELETS              =  42;
    static const int LARGE_CUT_GEMS         =  43;
    static const int ANVILS                 =  44;
    static const int BODY_PARTS             =  45;
    static const int REMAINS                =  46;
    static const int MEAT                   =  47;
    static const int FISH                   =  48;
    static const int SEEDS                  =  52;
    static const int PLANTS                 =  53;
    static const int LEATHER                =  54;
    static const int THREAD                 =  56;
    static const int CLOTH                  =  57;
    static const int LEGWEAR                =  59;
    static const int BACKPACKS              =  60;
    static const int QUIVERS                =  61;
    static const int CATAPULT_PARTS         =  62;
    static const int MECHANISMS             =  66;
    static const int TRAP_COMPONENTS        =  67;
    static const int DRINKS                 =  68;
    static const int POWDER                 =  69;
    static const int CHEESE                 =  70;
    static const int PREPARED_MEALS         =  71;
    static const int LIQUID                 =  72;
    static const int COINS                  =  73;
    static const int GLOB                   =  74;
    static const int PIPE_SECTIONS          =  76;
    static const int HATCH_COVERS           =  77;
    static const int GRATES                 =  78;
    static const int QUERNS                 =  79;
    static const int MILLSTONES             =  80;
    static const int SPLINTS                =  81;
    static const int CRUTCHES               =  82;
    static const int TRACTION_BENCHES       =  83;
    static const int TOOLS                  =  85;
    static const int SLABS                  =  86;
    static const int EGGS                   =  87;
    static const int BOOKS                  =  88;

    static const char* type2string(int type_id){
        switch(type_id){
            case BARS                  : return "bars";
            case SMALL_CUT_GEMS        : return "small cut gems";
            case BLOCKS                : return "blocks";
            case ROUGH_GEMS            : return "rough gems";
            case STONES                : return "stones";
            case LOGS                  : return "logs";
            case DOORS                 : return "doors";
            case FLOODGATES            : return "floodgates";
            case BEDS                  : return "beds";
            case THRONES               : return "thrones";
            case CHAINS_ROPES          : return "chains &amp; ropes";
            case FLASKS                : return "flasks";
            case GOBLETS               : return "goblets";
            case MUSICAL_INSTRUMENTS   : return "musical instruments";
            case TOYS                  : return "toys";
            case CAGES                 : return "cages";
            case BARRELS               : return "barrels";
            case BUCKETS               : return "buckets";
            case TABLES                : return "tables";
            case COFFINS               : return "coffins";
            case STATUES               : return "statues";
            case CORPSES               : return "corpses";
            case WEAPONS               : return "weapons";
            case BODYWEAR              : return "bodywear";
            case FOOTWEAR              : return "footwear";
            case SHIELDS               : return "shields";
            case HEADWEAR              : return "headwear";
            case HANDWEAR              : return "handwear";
            case BOXES_BAGS            : return "boxes &amp; bags";
            case BINS                  : return "bins";
            case ARMOR_STANDS          : return "armor stands";
            case WEAPON_RACKS          : return "weapon racks";
            case CABINETS              : return "cabinets";
            case FIGURINES             : return "figurines";
            case AMULETS               : return "amulets";
            case SCEPTERS              : return "scepters";
            case AMMUNITION            : return "ammunition";
            case CROWNS                : return "crowns";
            case RINGS                 : return "rings";
            case EARRINGS              : return "earrings";
            case BRACELETS             : return "bracelets";
            case LARGE_CUT_GEMS        : return "large cut gems";
            case ANVILS                : return "anvils";
            case BODY_PARTS            : return "body parts";
            case REMAINS               : return "remains";
            case MEAT                  : return "meat";
            case FISH                  : return "fish";
            case SEEDS                 : return "seeds";
            case PLANTS                : return "plants";
            case LEATHER               : return "leather";
            case THREAD                : return "thread";
            case CLOTH                 : return "cloth";
            case LEGWEAR               : return "legwear";
            case BACKPACKS             : return "backpacks";
            case QUIVERS               : return "quivers";
            case CATAPULT_PARTS        : return "catapult parts";
            case MECHANISMS            : return "mechanisms";
            case TRAP_COMPONENTS       : return "trap components";
            case DRINKS                : return "drinks";
            case POWDER                : return "powder";
            case CHEESE                : return "cheese";
            case PREPARED_MEALS        : return "prepared meals";
            case LIQUID                : return "liquid";
            case COINS                 : return "coins";
            case GLOB                  : return "glob";
            case PIPE_SECTIONS         : return "pipe sections";
            case HATCH_COVERS          : return "hatch covers";
            case GRATES                : return "grates";
            case QUERNS                : return "querns";
            case MILLSTONES            : return "millstones";
            case SPLINTS               : return "splints";
            case CRUTCHES              : return "crutches";
            case TRACTION_BENCHES      : return "traction benches";
            case TOOLS                 : return "tools";
            case SLABS                 : return "slabs";
            case EGGS                  : return "eggs";
            case BOOKS                 : return "books";
        }
        static char buf[0x100];
        sprintf(buf, "type %d", type_id);
        return buf;
    }
};
