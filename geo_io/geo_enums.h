#ifndef GEO_ENUMS
#define GEO_ENUMS

#include <vector>
#include <string>

struct geo_enum_element{
	int i;
	std::string str;
};
struct geo_enum : std::vector<geo_enum_element>{
	geo_enum(int _def, int n, ...);
        int str2int(const std::string & str) const;
        std::string int2str(const int i) const;
	int def;
};

extern const geo_enum wpt_map_displ_enum;
extern const geo_enum wpt_pt_dir_enum;
extern const geo_enum trk_type_enum;
extern const geo_enum trk_fill_enum;
extern const geo_enum wpt_symb_enum;
#endif
