/* 
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include "enc_dec_common.h"

#ifndef QMC_HEADER_INCLUDED


void encode_cblk(
   Jpeg2000T1Context *t1, 	// context
   MqcState *mqc,
   Jpeg2000Cblk *cblk,          // code block
   int width, 			// sirka 
   int height, 			// vyska
   int bandpos );               



#endif // QMC_HEADER_INCLUDED
#define QMC_HEADER_INCLUDED
