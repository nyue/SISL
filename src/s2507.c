/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* (c) Copyright 1995 by                                                     */
/*     SINTEF, Oslo, Norway                                                  */
/*     All rights reserved. See the copyright.h for more details.            */
/*                                                                           */
/*****************************************************************************/

#include "copyright.h"

/*
 *
 * $Id: s2507.c,v 1.2 1995-01-23 12:36:42 pfu Exp $
 *
 */


#define S2507

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void
s2507(SISLSurf *surf, int ider, double derive[], double normal[],
      double *totalCurvature, int *jstat)
#else
 void s2507(surf, ider, derive, normal, totalCurvature, jstat)
      SISLSurf *surf;
      int ider;
      double parvalue[];
      double derive[],
      double normal[],
      double *totalCurvature;
      int *jstat;
#endif
/*
***************************************************************************
*
***************************************************************************
*  PURPOSE      :  To compute the total curvature T(u,v) of a Surface
*                  for given values (u,v). This is a lower level routine,
*                  used for evaluation of many T(u,v)'s.
*
*  INPUT        :
*          surf     - Pointer to the surface to evaluate.
*          ider     - Not used.
*       derive      - Array containing derivatives from routine s1421().
*                     Size = idim*6.
*       normal      - Array containing the normal from routine s1421().
*                     Size = 3.
*
*  INPUT/OUTPUT :
*
*  OUTPUT       :
*    totalCurvature - Total curvature of the surface in (u,v).
*        jstat      - Staus messages
*                         = 0 : Ok.
*                         < 0 : Error.
*
*  METHOD        :  The total curvature is given by
*
*                      T(x,y) = k1^2 + k2^2,
*
*                  if the surface (h(x,y)) is 1D, and
*
*                      T(u,v) = = k1^2 + k2^2,
*
*                  if the surface (X(u,v)) is 3D. The variables E,F,G,e,f and g
*                  are the coefficients of the first and second fundamental form.
*                  They are given by: e = <N,Xuu>, f = <N,Xuv>, g = <N,Xvv>,
*                  E = <Xu,Xu>, F = <Xu,Xv> and G = <Xv,Xv>.
*
*  REFERENCES   :  Differential Geometry of Curves and Surfaces,
*                    (Manfredo P. Do Carmo, Prentice Hall,
*                      ISBN: 0-13-212589-7).
*-
*  CALLS        :
*
*  LIMITATIONS  :
*                (i) If the surface is degenerated (not regular) at the point
*                    (u,v), it makes now sense to speak about T(u,v).
*               (ii) If the surface is closed to degenerate, T(u,v)
*                    can be numerical unstable.
*              (iii) The surface should be C2, since the total c. is calculated
*                    from the second derivatives.
*               (iv) The dimension of the space in which the surface lies must
*                    be 1,2 or 3.  The routine return jstat < 0.
*
*
* WRITTEN BY :  Geir Westgaard, SINTEF, Oslo, Norway.            Date: 1995-1
*****************************************************************************
*/
{
  double a,b;             /* Dummy variables.                                */
  double hx,hy,
    hxx,hyy,hxy;         /* The derivatives of the 1D surface, h(x,y).      */
  double E,F,G;           /* The coefficents of the first fundamental form,
			     that is, E = <Xu,Xu>, F = <Xu,Xv>  and
			     G = <Xv,Xv>.                                    */
  double e,f,g;           /* The coefficents of the second fundamental form,
			     that is, e = <N,Xuu>, f = <N,Xuv> and
			     g = <N,Xvv>.                                    */
  double gc;              /* Gaussian curvature.                             */
  double mc;              /* Mean curvature.                                 */
  double k1,k2;           /* Max. and min. principal curvature.              */



  if (surf->idim == 1) /* 1D surface */
  {
    hx  = derive[1];
    hy  = derive[2];
    hxx = derive[3];
    hxy = derive[4];
    hyy = derive[5];


    a = (1+hx*hx+hy*hy);
    gc = (hxx*hyy-hxy*hxy)/(a*a);

    b = sqrt(a*a*a);

    a = (1.0 + hx*hx)*hyy - 2.0*hx*hy*hxy + (1.0 + hy*hy)*hxx;

    mc = a/b;

    a = sqrt(mc*mc - gc);
    k1 = mc + a;
    k2 = mc - a;

    *totalCurvature = k1*k1 + k2*k2;
  }

  else if (surf->idim == 2) /* 2D surface */
  {
    /* The surface lies in a plane => T(u,v) = 0 */

    *totalCurvature = 0.0;
  }
  else if (surf->idim == 3) /* 3D surface */
  {
    /* E = <Xu,Xu> */
    E = derive[3]*derive[3]+derive[4]*derive[4]+derive[5]*derive[5];

	 /* F = <Xu,Xv> */
    F = derive[3]*derive[6]+derive[4]*derive[7]+derive[5]*derive[8];

    /* G = <Xv,Xv> */
    G = derive[6]*derive[6]+derive[7]*derive[7]+derive[8]*derive[8];

    /* b = EG + F^2. */
    b = E*G-F*F;

    /* e = <N,Xuu> (/ sqrt(E*G-F*F)) */
    e = normal[0]*derive[9]+normal[1]*derive[10]+normal[2]*derive[11];

    /* f = <N,Xuv> (/ sqrt(E*G-F*F)) */
    f = normal[0]*derive[12]+normal[1]*derive[13]+normal[2]*derive[14];

    /* g = <N,Xvv> (/ sqrt(E*G-F*F)) */
    g = normal[0]*derive[15]+normal[1]*derive[16]+normal[2]*derive[17];

    /* Compute total curvature = k1^2 + k2^2. */

    gc = (e*g-f*f)/(b*b);

    a = 0.5*(e*G - 2.0*f*F + g*E);
    b = b*sqrt(b);

    mc = a/b;

    a = sqrt(mc*mc - gc);
    k1 = mc + a;
    k2 = mc - a;

    *totalCurvature = k1*k1 + k2*k2;

  }
  else /* When surf->idim != 1,2 or 3 */
  {
    goto err105;
  }




  /* Successful computations  */

  *jstat = 0;
  goto out;


   /* Error in input, surf->idim != 1,2 or 3 */
err105:
  *jstat = -105;
  s6err("s2507", *jstat, 0);
  goto out;

out:

  return;

}