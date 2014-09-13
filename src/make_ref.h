#ifndef make_ref_h
#define make_ref_h

#include <osg/ref_ptr>

template <class T, class... Args>
osg::ref_ptr<T> make_ref (Args&&... args)
{
    return osg::ref_ptr<T>(new T(args...));
};

#endif // make_ref_h