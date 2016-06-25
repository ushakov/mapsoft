#ifndef WIDGETS_COMBOBOXES_H
#define WIDGETS_COMBOBOXES_H

#include "simple_combo.h"
#include "geo/geo_convs.h"

struct CBProj   : public SimpleCombo<Proj>   { CBProj(); };
struct CBDatum  : public SimpleCombo<Datum>  { CBDatum(); };
struct CBScale  : public SimpleCombo<int>    { CBScale(); };
struct CBUnit   : public SimpleCombo<int>    { CBUnit(); };
struct CBPage   : public SimpleCombo<iPoint> { CBPage(); };
struct CBCorner : public SimpleCombo<int>    { CBCorner(); };
struct CBSrtmW  : public SimpleCombo<int>    { CBSrtmW(); };

#endif
