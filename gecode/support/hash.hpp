/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *     Guido Tack <tack@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2018
 *     Guido Tack, 2018
 *
 *  Last modified:
 *     $Date$ by $Author$
 *     $Revision$
 *
 *  This file is part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

namespace Gecode {

  /// Combine hash value \a h into \a seed
  void cmb_hash(std::size_t& seed, std::size_t h);
  /// Combine hash value \a h into \a seed
  void cmb_hash(std::size_t& seed, int h);
  /// Combine hash value \a h into \a seed
  void cmb_hash(std::size_t& seed, unsigned int h);


  forceinline void
  cmb_hash(std::size_t& seed, std::size_t h) {
    seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  forceinline void
  cmb_hash(std::size_t& seed, int h) {
    cmb_hash(seed, static_cast<std::size_t>(h));
  }

  forceinline void
  cmb_hash(std::size_t& seed, unsigned int h) {
    cmb_hash(seed, static_cast<std::size_t>(h));
  }

}

// STATISTICS: support-any
