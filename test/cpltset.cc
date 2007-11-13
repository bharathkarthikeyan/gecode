/*
 *  Main authors:
 *     Guido Tack <tack@gecode.org>
 *     Christian Schulte <schulte@gecode.org>
 *     Mikael Lagerkvist <lagerkvist@gecode.org>
 *
 *  Copyright:
 *     Guido Tack, 2005
 *     Christian Schulte, 2005
 *     Mikael Lagerkvist, 2005
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

#include "test/cpltset.hh"

#include "gecode/int.hh"
#include "gecode/iter.hh"
#include <algorithm>

  /// Check the test result and handle failed test
#define CHECK_TEST(T,M)                                         \
if (opt.log)                                                    \
  olog << ind(3) << "Check: " << (M) << std::endl;              \
if (!(T)) {                                                     \
  problem = (M); delete s; goto failed;                         \
}

  /// Start new test
#define START_TEST(T)                                           \
  if (opt.log) {                                                \
     olog.str("");                                              \
     olog << ind(2) << "Testing: " << (T) << std::endl;         \
  }                                                             \
  test = (T);

namespace Test { namespace CpltSet {

  /// Space for executing CpltSet tests
  class CpltSetTestSpace : public Gecode::Space {
  public:
    /// Set variables to be tested
    Gecode::CpltSetVarArray x;
    /// Int variables to be tested
    Gecode::IntVarArray y;
    /// How many integer variables are used by the test
    int withInt;
    /// Control variable for reified propagators
    Gecode::BoolVar b;
    /// Whether the test is for a reified propagator
    bool reified;
    /// The test currently run
    CpltSetTest* test;
    /// The options
    const Options opt;

  public:
    /**
      * \brief Create test space
      *
      * Creates \a n set variables with domain \a d,
      * \a i integer variables with domain \a d, and stores whether
      * the test is for a refied propagator (\a r), the test itself
      * (\a t) and the options (\a o). 
      * 
      */
    CpltSetTestSpace(int n, Gecode::IntSet& d, int i, bool r, CpltSetTest* t,
                     const Options& o, bool log=true)
      : x(this, n, Gecode::IntSet::empty, d), y(this, i, d), withInt(i),
        b(this, 0, 1), reified(r), test(t), opt(o) {
      if (opt.log && log) {
        olog << ind(2) << "Initial: x[]=" << x;
        olog << " y[]=" << y;
        if (reified)
          olog << " b=" << b;
        olog << std::endl;
      }
    }

    /// Constructor for cloning \a s
    CpltSetTestSpace(bool share, CpltSetTestSpace& s) 
      : Space(share,s), withInt(s.withInt), reified(s.reified), test(s.test), 
        opt(s.opt) {
      x.update(this, share, s.x);
      y.update(this, share, s.y);
      b.update(this, share, s.b);
    }
  
    /// Copy space during cloning
    virtual Space* copy(bool share) {
      return new CpltSetTestSpace(share,*this);
    }

    /// Post propagator
    void post(void) {
      if (reified){
        test->post(this,x,y,b);
        if (opt.log)
          olog << ind(3) << "Posting reified propagator" << std::endl;
      } else {
        test->post(this,x,y);
        if (opt.log)
          olog << ind(3) << "Posting propagator" << std::endl;
      }
    }
    /// Compute a fixpoint and check for failure
    bool failed(void) {
      if (opt.log) {
        olog << ind(3) << "Fixpoint: x[]=" << x
             << " y[]=" << y << std::endl;
        bool f=(status() == Gecode::SS_FAILED);
        olog << ind(3) << "     -->  x[]=" << x 
             << " y[]=" << y << std::endl;
        return f;
      } else {
        return status() == Gecode::SS_FAILED;
      }
    }

    /// Perform set tell operation on \a x[i]
    void rel(int i, Gecode::SetRelType srt, const Gecode::IntSet& is) {
      if (opt.log) {
        olog << ind(4) << "x[" << i << "] ";
        switch (srt) {
        case Gecode::SRT_EQ: olog << "="; break;
        case Gecode::SRT_NQ: olog << "!="; break;
        case Gecode::SRT_SUB: olog << "<="; break;
        case Gecode::SRT_SUP: olog << ">="; break;
        case Gecode::SRT_DISJ: olog << "||"; break;
        case Gecode::SRT_CMPL: olog << "^-1 = "; break;
        }
        olog << is << std::endl;
      }
      Gecode::dom(this, x[i], srt, is);
    }
    /// Perform cardinality tell operation on \a x[i]
    void cardinality(int i, int cmin, int cmax) {
      if (opt.log) {
        olog << ind(4) << cmin << " <= #(x[" << i << "]) <= " << cmax
             << std::endl;
      }
      Gecode::cardinality(this, x[i], cmin, cmax);
    }
    /// Perform integer tell operation on \a y[i]
    void rel(int i, Gecode::IntRelType irt, int n) {
      if (opt.log) {
        olog << ind(4) << "y[" << i << "] ";
        switch (irt) {
        case Gecode::IRT_EQ: olog << "="; break;
        case Gecode::IRT_NQ: olog << "!="; break;
        case Gecode::IRT_LQ: olog << "<="; break;
        case Gecode::IRT_LE: olog << "<"; break;
        case Gecode::IRT_GQ: olog << ">="; break;
        case Gecode::IRT_GR: olog << ">"; break;
        }
        olog << " " << n << std::endl;
      }
      Gecode::rel(this, y[i], irt, n);
    }
    /// Perform Boolean tell on \a b
    void rel(bool sol) {
      int n = sol ? 1 : 0;
      assert(reified);
      if (opt.log) 
        olog << ind(4) << "b = " << n << std::endl;
      Gecode::rel(this, b, Gecode::IRT_EQ, n);
    }

    /// Assign all variables to values in \a a
    void assign(const Test::Set::SetAssignment& a) {
      for (int i=a.size(); i--; ) {
        Test::Set::CountableSetRanges csv(a.lub, a[i]);
        Gecode::IntSet ai(csv);
        rel(i, Gecode::SRT_EQ, ai);
        if (Base::fixpoint() && failed())
          return;
      }
      for (int i=withInt; i--; ) {
        rel(i, Gecode::IRT_EQ, a.ints()[i]);
        if (Base::fixpoint() && failed())
          return;
      }
    }

    /// Test whether all variables are assigned
    bool assigned(void) const {
      for (int i=x.size(); i--; )
        if (!x[i].assigned())
          return false;
      for (int i=y.size(); i--; )
        if (!y[i].assigned())
          return false;
      return true;
    }

    void removeFromLub(int v, int i, const Test::Set::SetAssignment& a) {
      Gecode::CpltSetVarUnknownRanges ur(x[i]);
      Test::Set::CountableSetRanges air(a.lub, a[i]);
      Gecode::Iter::Ranges::Diff<Gecode::CpltSetVarUnknownRanges, 
        Test::Set::CountableSetRanges> diff(ur, air);
      Gecode::Iter::Ranges::ToValues<Gecode::Iter::Ranges::Diff
        <Gecode::CpltSetVarUnknownRanges, Test::Set::CountableSetRanges> > 
          diffV(diff);
      for (int j=0; j<v; j++, ++diffV);
      rel(i, Gecode::SRT_DISJ, Gecode::IntSet(diffV.val(), diffV.val()));
    }

    void addToGlb(int v, int i, const Test::Set::SetAssignment& a) {
      Gecode::CpltSetVarUnknownRanges ur(x[i]);
      Test::Set::CountableSetRanges air(a.lub, a[i]);
      Gecode::Iter::Ranges::Inter<Gecode::CpltSetVarUnknownRanges, 
        Test::Set::CountableSetRanges> inter(ur, air);
      Gecode::Iter::Ranges::ToValues<Gecode::Iter::Ranges::Inter
        <Gecode::CpltSetVarUnknownRanges, Test::Set::CountableSetRanges> > 
          interV(inter);
      for (int j=0; j<v; j++, ++interV);
      rel(i, Gecode::SRT_SUP, Gecode::IntSet(interV.val(), interV.val()));
    }

    bool fixprob(void) {
      if (failed())
        return true;
      CpltSetTestSpace* c = static_cast<CpltSetTestSpace*>(clone());
      if (opt.log)
        olog << ind(3) << "Testing fixpoint on copy" << std::endl;
      c->post();
      if (c->failed()) {
        delete c; return false;
      }

      for (int i=x.size(); i--; )
        if (Gecode::CpltSet::CpltSetView(x[i]).dom() != 
            Gecode::CpltSet::CpltSetView(c->x[i]).dom()) {
          delete c;
          return false;
        }
      for (int i=y.size(); i--; )
        if (y[i].size() != c->y[i].size()) {
          delete c; return false;
        }
      if (reified && (b.size() != c->b.size())) {
        delete c; return false;
      }
      if (opt.log)
        olog << ind(3) << "Finished testing fixpoint on copy" << std::endl;
      delete c;
      return true;
    }

    bool prune(const Test::Set::SetAssignment& a) {
      bool setsAssigned = true;
      for (int j=x.size(); j--; )
        if (!x[j].assigned()) {
          setsAssigned = false;
          break;
        }
      bool intsAssigned = true;
      for (int j=y.size(); j--; )
        if (!y[j].assigned()) {
          intsAssigned = false;
          break;
        }

      // Select variable to be pruned
      int i;
      if (intsAssigned) {
        i = Base::rand(x.size());
      } else if (setsAssigned) {
        i = Base::rand(y.size());
      } else {
        i = Base::rand(x.size()+y.size());
      }

      if (setsAssigned || i>=x.size()) {
        if (i>=x.size())
          i = i-x.size();
        while (y[i].assigned()) {
          i = (i+1) % y.size();
        }
        // Prune int var

        // Select mode for pruning
        switch (Base::rand(3)) {
        case 0:
          if (a.ints()[i] < y[i].max()) {
            int v=a.ints()[i]+1+
              Base::rand(static_cast<unsigned int>(y[i].max()-a.ints()[i]));
            assert((v > a.ints()[i]) && (v <= y[i].max()));
            rel(i, Gecode::IRT_LE, v);
          }
          break;
        case 1:
          if (a.ints()[i] > y[i].min()) {
            int v=y[i].min()+
              Base::rand(static_cast<unsigned int>(a.ints()[i]-y[i].min()));
            assert((v < a.ints()[i]) && (v >= y[i].min()));
            rel(i, Gecode::IRT_GR, v);
          }
          break;
        default:
          int v;
          Gecode::Int::ViewRanges<Gecode::Int::IntView> it(y[i]);
          unsigned int skip = Base::rand(y[i].size()-1);
          while (true) {
            if (it.width() > skip) {
              v = it.min() + skip;
              if (v == a.ints()[i]) {
                if (it.width() == 1) {
                  ++it; v = it.min();
                } else if (v < it.max()) {
                  ++v;
                } else {
                  --v;
                }
              }
              break;
            }
            skip -= it.width();
            ++it;
          }
          rel(i, Gecode::IRT_NQ, v);
        }
        return (!Base::fixpoint() || fixprob());
      }
      while (x[i].assigned()) {
        i = (i+1) % x.size();
      }
      Gecode::CpltSetVarUnknownRanges ur1(x[i]);
      Test::Set::CountableSetRanges air1(a.lub, a[i]);
      Gecode::Iter::Ranges::Diff<Gecode::CpltSetVarUnknownRanges, 
        Test::Set::CountableSetRanges> diff(ur1, air1);
      Gecode::CpltSetVarUnknownRanges ur2(x[i]);
      Test::Set::CountableSetRanges air2(a.lub, a[i]);
      Gecode::Iter::Ranges::Inter<Gecode::CpltSetVarUnknownRanges, 
        Test::Set::CountableSetRanges> inter(ur2, air2);

      Test::Set::CountableSetRanges aisizer(a.lub, a[i]);
      unsigned int aisize = Gecode::Iter::Ranges::size(aisizer);

      // Select mode for pruning
      switch (Base::rand(5)) {
      case 0:
        if (inter()) {
          int v = Base::rand(Gecode::Iter::Ranges::size(inter));
          addToGlb(v, i, a);
        }
        break;
      case 1:
        if (diff()) {
          int v = Base::rand(Gecode::Iter::Ranges::size(diff));
          removeFromLub(v, i, a);
        }
        break;
      case 2:
        if (x[i].cardMin() < aisize) {
          unsigned int newc = x[i].cardMin() + 1 +
            Base::rand(aisize - x[i].cardMin());
          assert( newc > x[i].cardMin() );
          assert( newc <= aisize );
          cardinality(i, newc, Gecode::Limits::Set::card_max);
        }
        break;
      case 3:
        if (x[i].cardMax() > aisize) {
          unsigned int newc = x[i].cardMax() - 1 -
            Base::rand(x[i].cardMax() - aisize);
          assert( newc < x[i].cardMax() );
          assert( newc >= aisize );
          cardinality(i, 0, newc);
        }
        break;
      default:
        if (inter()) {
          int v = Base::rand(Gecode::Iter::Ranges::size(inter));
          addToGlb(v, i, a);
        } else {
          int v = Base::rand(Gecode::Iter::Ranges::size(diff));
          removeFromLub(v, i, a);
        }      
      }
      return (!Base::fixpoint() || fixprob());
    }
  };

  bool 
  CpltSetTest::run(void) {
    const char* test    = "NONE";
    const char* problem = "NONE";

    Test::Set::SetAssignment* ap =
      new Test::Set::SetAssignment(arity,lub,withInt);
    Test::Set::SetAssignment& a = *ap;

    int an = 0;
  
    while (a()) {
      bool is_sol = solution(a);
      if (opt.log)
        olog << ind(1) << "Assignment: " << a 
             << (is_sol ? " (solution)" : " (no solution)")
             << std::endl;
    
      START_TEST("Assignment (after posting)");
      {
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,false,this,opt);
        s->post();
        s->assign(a);
        if (is_sol) {
          CHECK_TEST(!s->failed(), "Failed on solution");
          CHECK_TEST(s->propagators()==0, "No subsumtion");
        } else {
          CHECK_TEST(s->failed(), "Solved on non-solution");
        }
        delete s;
      }
    
      START_TEST("Assignment (before posting)");
      {
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,false,this,opt);
        s->assign(a);
        s->post();
        if (is_sol) {
          CHECK_TEST(!s->failed(), "Failed on solution");
          CHECK_TEST(s->propagators()==0, "No subsumtion");
        } else {
          CHECK_TEST(s->failed(), "Solved on non-solution");
        }
        delete s;
      }
    
      if (reified) {
        START_TEST("Assignment reified (before posting)");
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,true,this,opt);
        s->assign(a);
        s->post();
        CHECK_TEST(!s->failed(), "Failed");
        CHECK_TEST(s->propagators()==0, "No subsumtion");
        CHECK_TEST(s->b.assigned(), "Control variable unassigned");
        if (is_sol) {
          CHECK_TEST(s->b.val()==1, "Zero on solution");
        } else {
          CHECK_TEST(s->b.val()==0, "One on non-solution");
        }
        delete s;
      }
    
      if (reified) {
        START_TEST("Assignment reified (after posting)");
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,true,this,opt);
        s->post();
        s->assign(a);
        CHECK_TEST(!s->failed(), "Failed");
        CHECK_TEST(s->propagators()==0, "No subsumtion");
        CHECK_TEST(s->b.assigned(), "Control variable unassigned");
        if (is_sol) {
          CHECK_TEST(s->b.val()==1, "Zero on solution");
        } else {
          CHECK_TEST(s->b.val()==0, "One on non-solution");
        }
        delete s;
      }
    
      START_TEST("Prune");
      {
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,false,this,opt);
        s->post();
        while (!s->failed() && !s->assigned())
           if (!s->prune(a)) {
             problem = "No fixpoint";
             delete s;
             goto failed;
           }
        s->assign(a);
        if (is_sol) {
          CHECK_TEST(!s->failed(), "Failed on solution");
          CHECK_TEST(s->propagators()==0, "No subsumtion");
        } else {
          CHECK_TEST(s->failed(), "Solved on non-solution");
        }
        delete s;
      }
    
      if (reified) {
        START_TEST("Prune reified");
        Gecode::CpltSet::manager.dispose();
        Gecode::CpltSet::manager.init(10000,10000);
        CpltSetTestSpace* s =
          new CpltSetTestSpace(arity,lub,withInt,true,this,opt);
        s->post();
        while (!s->assigned() && !s->b.assigned())
           if (!s->prune(a)) {
             problem = "No fixpoint";
             delete s;
             goto failed;
           }
        CHECK_TEST(!s->failed(), "Failed");
        CHECK_TEST(s->propagators()==0, "No subsumtion");
        CHECK_TEST(s->b.assigned(), "Control variable unassigned");
        if (is_sol) {
          CHECK_TEST(s->b.val()==1, "Zero on solution");
        } else {
          CHECK_TEST(s->b.val()==0, "One on non-solution");
        }
        delete s;
      }

      ++an;
      ++a;
    }

    delete ap;
    return true;
   failed:
    if (opt.log)
      olog << "FAILURE" << std::endl
	   << ind(1) << "Test:       " << test << std::endl
	   << ind(1) << "Problem:    " << problem << std::endl;
    if (a() && opt.log)
      olog << ind(1) << "Assignment: " << a << std::endl;
    delete ap;

    return false;
  }

}}

// STATISTICS: test-cpltset
