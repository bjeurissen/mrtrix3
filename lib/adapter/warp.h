/*
   Copyright 2009 Brain Research Institute, Melbourne, Australia

   Written by David Raffelt 2015

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __adapter_warp_h__
#define __adapter_warp_h__

#include "image.h"
#include "transform.h"
#include "registration/transform/compose.h"
#include "interp/cubic.h"

namespace MR
{
  namespace Adapter
  {

    extern const transform_type NoTransform;

    //! \addtogroup interp
    // @{

    //! an Image providing interpolated values from another Image
    /*! the Warp class provides an Image interface to data
     * interpolated ....
     *
     * For example:
     * \code
     * // reference header:
     * auto reference = Header::open (argument[0]);
     * // input data to be resliced:
     * auto input = Image<float>::open (argument[1]);
     *
     * \endcode
     *
     *
     * \sa Interp::warp()
     */
    template <template <class ImageType> class Interpolator, class ImageType, class WarpType>
      class Warp
    {
      public:
        typedef typename ImageType::value_type value_type;

          Warp (const ImageType& original,
                const WarpType& warp,
                const value_type value_when_out_of_bounds = Transform::default_out_of_bounds_value<value_type>()) :
            interp (original, value_when_out_of_bounds),
            warp (warp),
            x { 0, 0, 0 },
            dim { warp.size(0), warp.size(1), warp.size(2) },
            vox { warp.voxsize(0), warp.voxsize(1), warp.voxsize(2) } {
              assert (warp.ndim() == 4);
              assert (warp.size(3) == 3);
            }


        size_t ndim () const { return interp.ndim(); }
        int size (size_t axis) const { return axis < 3 ? dim[axis]: interp.size (axis); }
        default_type voxsize (size_t axis) const { return axis < 3 ? vox[axis] : interp.voxsize (axis); }
        const std::string& name () const { return interp.name(); }

        ssize_t stride (size_t axis) const {
          return interp.stride (axis);
        }

        void reset () {
          x[0] = x[1] = x[2] = 0;
          for (size_t n = 3; n < interp.ndim(); ++n)
            interp.index(n) = 0;
        }

        value_type value () {
          warp.index(0) = x[0];
          warp.index(1) = x[1];
          warp.index(2) = x[2];

          Eigen::Vector3d position = warp.row(3).template cast<double>();
          interp.scanner (position);
          return interp.value();
        }

        ssize_t index (size_t axis) const { return axis < 3 ? x[axis] : interp.index(axis); }
        auto index (size_t axis) -> decltype(Helper::index(*this, axis)) { return { *this, axis }; }
        void move_index (size_t axis, ssize_t increment) {
          if (axis < 3) x[axis] += increment;
          else interp.index(axis) += increment;
        }

      private:
        Interpolator<ImageType> interp;
        WarpType warp;
        ssize_t x[3];
        const ssize_t dim[3];
        const default_type vox[3];
    };

    //! @}

  }
}

#endif




