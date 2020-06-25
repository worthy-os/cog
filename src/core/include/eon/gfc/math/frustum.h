//------------------------------------------------------------------------------
//       Copyright 2014-2019 Creepy Doll Games LLC. All rights reserved.
//
//                  The best method for accelerating a computer
//                     is the one that boosts it by 9.8 m/s2.
//------------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
// NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

//================================================|=============================
//Frustum:{                                       |

	/** \brief Frustum class.
		*
		* The frustum is generally used for culling objects in a scene. There is
		* one attached to the sg camera.
		*/

	struct Frustum final{

    //--------------------------------------------|-----------------------------
    //Methods:{                                   |

      /** \brief Setup the frustum.
        *
        * This routine will setup the viewing frustum.
        *
        * \param W2C Matrix to build the frustum from.
        */

      void set( const Matrix4& W2C );

      /** \brief Is point visible.
        *
        * This routine will check to see if the given point is on the inside of
        * all planes in the frustum.
        *
        * \param P The point to test.
        *
        * \return Returns if the point is inside the frustum.
        */

      bool intersects( const Point3& P )const;

      /** \brief Is sphere visible.
        *
        * This routine will check to see if the given sphere is on the inside
        * of all planes in the frustum.
        *
        * \param S The sphere to test.
        *
        * \return Returns if the frustum intersects the given sphere.
        */

      bool intersects( const Sphere& S )const;

      /** \brief Is bounds visible.
        *
        * This routine will check to see if the given bounds is on the inside
        * of all planes in the frustum.
        *
        * \param B The bounding box to test.
        */

      bool intersects( const AABB3& B )const;

      /** \brief Set a plane.
        *
        * This routine will set the given plane.
        *
        * \param i The plane index (0-5).
        * \param p The plane.
        */

      e_forceinline void setPlane( const s32 i, const Plane& p ){
        e_assert( i < e_dimof( m_aPlanes ));
        m_aPlanes[i] = p;
      }

      /** \brief Get a plane.
        *
        * This routine will return the plane at the given index.
        *
        * \param i The index to the desired plane.
        *
        * \return Returns a reference to the requested plane.
        */

      e_forceinline Plane& toPlane( const s32 i ){
        e_assert( i < e_dimof( m_aPlanes ));
        return m_aPlanes[i];
      }

    //}:                                          |
    //--------------------------------------------|-----------------------------

    Frustum() = default;

	private:

		Plane m_aPlanes[6];
	};

//}:                                              |
//================================================|=============================