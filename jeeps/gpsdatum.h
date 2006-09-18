#ifdef __cplusplus
extern "C"
{
#endif

#ifndef gpsdatum_h
#define gpsdatum_h



typedef struct GPS_SEllipse
{
    char   *name;
    double a;
    double invf;
} GPS_OEllipse, *GPS_PEllipse;

extern GPS_OEllipse GPS_Ellipse[];

typedef struct GPS_SDatum
{
    char   *name;
    int    ellipse;
    double dx;
    double dy;
    double dz;
} GPS_ODatum, *GPS_PDatum;

extern GPS_ODatum GPS_Datum[];


/* UK Ordnance Survey Nation Grid Map Codes */
extern char *UKNG[];



#endif

#ifdef __cplusplus
}
#endif
