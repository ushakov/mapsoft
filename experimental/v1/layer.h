#ifndef LAYER_H
#define LAYER_H

#include <boost/operators.hpp>

#include <lib2d/image.h>
#include <options/options.h>
#include <libgeo/geo_data.h>

/// Растровый слой -- абстрактный класс.
/// Знает, как отдать некоторую растровую картинку в прямоугольных
/// целочисленных координатах. Рассчитан на неоднократное обращение за
/// маленькими кусочками

/// Слой без геодезической привязки должен использовать scale + shift,
/// слой с привязкой должен уметь get_ref + set_ref.

/// Для перемасштабирования и сдвига слоев должны использоваться операторы
/// * / - +.

class Layer:
  public boost::multiplicative<Layer,double>,
  public boost::additive<Layer, dPoint>
 {
private:
    dPoint shift;
    double scale;

public:

    virtual iImage get_image (iRect src) = 0;
    virtual iRect range() = 0;
    virtual void refresh() = 0;

    /// Gets current layer configuration as Options
    /// Default implementation returns empty Options
    virtual Options get_config() { return Options(); }
    /// Gets layer configuration from Options
    /// Default implementation does nothing
    virtual void set_config(const Options& opt) {  }

    virtual Layer & operator/= (double k) {scale/=k; set_ref(get_ref()/k);}
    virtual Layer & operator*= (double k) {scale*=k; set_ref(get_ref()*k);}
    virtual Layer & operator-= (dPoint k) {shift-=k; set_ref(get_ref()-k);}
    virtual Layer & operator+= (dPoint k) {shift+=k; set_ref(get_ref()+k);}

    virtual void rescale(){scale=1; shift=iPoint(0,0); set_ref();}


    virtual void set_scale(double s){scale=s;}
    virtual void set_shift(dPoint s){shift=s;}
    virtual double get_scale() const {return scale;}
    virtual dPoint get_shift() const {return shift;}

    // Get current geo reference (empty by default)
    virtual g_map get_ref() const { return g_map();}
    // Set geo reference (do nothing by default)
    virtual void set_ref(const g_map & map){}
    // Set native geo reference (do nothing by default)
    virtual void set_ref(){}


    Layer():scale(1.0), shift(0.0,0.0){}
    Layer (Layer const & other) { }
    virtual void swap (Layer & other) { }
};


#endif /* LAYER_H */
