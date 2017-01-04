/**
 * @file   ProviderList_test.cc
 * @brief  Unit test for ProviderList.h
 * @date   April 22nd, 2016
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @see    ProviderList.h
 * 
 * This test does not take any argument.
 * 
 */


// GArSoft libraries
#include "TestUtils/ProviderList.h"

// Boots libraries 
#define BOOST_TEST_MODULE ProviderList_test
#include <cetlib/quiet_unit_test.hpp>

// C/C++ standard library
#include <set>
#include <memory> // std::unique_ptr<>


//------------------------------------------------------------------------------

std::set<void*> TrackedMemory; // track allocated objects


//------------------------------------------------------------------------------
// mock up providers
//
struct NormalDatumClass {
   explicit NormalDatumClass(unsigned int v): value(v)
      { TrackedMemory.insert(this); }
   
   NormalDatumClass(NormalDatumClass const&) = default;
   NormalDatumClass(NormalDatumClass&&) = default;
   NormalDatumClass& operator= (NormalDatumClass const&) = default;
   NormalDatumClass& operator= (NormalDatumClass&&) = default;
   ~NormalDatumClass() { TrackedMemory.erase(this); }
   
   unsigned int value;
}; // class NormalDatumClass


struct UncopiableDatumClass {
   explicit UncopiableDatumClass(const char* v): value(v)
      { TrackedMemory.insert(this); }
   UncopiableDatumClass(UncopiableDatumClass const&) = delete;
   UncopiableDatumClass(UncopiableDatumClass&&) = default;
   UncopiableDatumClass& operator= (UncopiableDatumClass const&) = delete;
   UncopiableDatumClass& operator= (UncopiableDatumClass&&) = default;
   ~UncopiableDatumClass() { TrackedMemory.erase(this); }
   
   const char* value;
   
}; // class UncopiableDatumClass


struct UnmovableDatumClass {
   explicit UnmovableDatumClass()
      { TrackedMemory.insert(this); }
   UnmovableDatumClass(UnmovableDatumClass const&) = default;
   UnmovableDatumClass(UnmovableDatumClass&&) = delete;
   UnmovableDatumClass& operator= (UnmovableDatumClass const&) = default;
   UnmovableDatumClass& operator= (UnmovableDatumClass&&) = delete;
   ~UnmovableDatumClass() { TrackedMemory.erase(this); }
}; // class UnmovableDatumClass

namespace testing {
   
   template <>
   std::unique_ptr<UnmovableDatumClass> setupProvider<UnmovableDatumClass>() {
      BOOST_TEST_MESSAGE
        (" *** special setup for UnmovableDatumClass *** ");
      return std::make_unique<UnmovableDatumClass>();
   } // setupProvider<UnmovableDatumClass>()
   
} // namespace testing


struct PinnedDatumClassBase {
   virtual ~PinnedDatumClassBase() = default;
   
   virtual bool abstract() const = 0;
}; // PinnedDatumClassBase

struct PinnedDatumClass: public PinnedDatumClassBase {
   explicit PinnedDatumClass(float x, float y): value1(x), value2(y)
      { TrackedMemory.insert(this); }
   PinnedDatumClass(PinnedDatumClass const&) = delete;
   PinnedDatumClass(PinnedDatumClass&&) = delete;
   PinnedDatumClass& operator= (PinnedDatumClass const&) = delete;
   PinnedDatumClass& operator= (PinnedDatumClass&&) = delete;
   virtual ~PinnedDatumClass() { TrackedMemory.erase(this); }
   virtual bool abstract() const override { return false; }
   
   static std::unique_ptr<PinnedDatumClass> New(float x, float y)
      {
         ++nNewCalls;
         return
           testing::DefaultSetupProviderClass<PinnedDatumClass>::setup(x, y);
      }
   
   static unsigned int nNewCalls;
   
   float value1, value2;
}; // class PinnedDatumClass

unsigned int PinnedDatumClass::nNewCalls = 0;

//------------------------------------------------------------------------------
// test functions
//

template <typename T, typename LIST>
void TestElement(LIST& l, std::string label = "", bool present = true) {
   
   BOOST_TEST_MESSAGE("Testing class '" << typeid(T).name() << "' label \""
     << label << "\" (" << (present? "present": "not present") << ")" );
   BOOST_CHECK_EQUAL(l.template known<T>(label), present);
   BOOST_CHECK_EQUAL(l.template valid<T>(label), present);
   try {
      auto const& item = l.template get<T>(label);
      BOOST_CHECK(present); // if not present, we should not be here!
      BOOST_CHECK(&item);
   }
   catch (testing::ProviderList::provider_not_available const& e) {
      BOOST_CHECK(!present); // if present, we should not be here!
   }
   
} // TestElement()


template <typename LIST>
void TestBase(LIST& l) {
   
   TestElement<NormalDatumClass>(l);
   TestElement<UncopiableDatumClass>(l, "", false); // empty label not present
   TestElement<UncopiableDatumClass>(l, "One"); // label "One" present
   TestElement<UncopiableDatumClass>(l, "Two", false); // label "Two" not there
   TestElement<UnmovableDatumClass>(l);
   TestElement<PinnedDatumClass>(l);
   TestElement<PinnedDatumClassBase>(l);
   TestElement<int>(l, "", false); // no int present
   
   // test that the polymorphism is preserved by aliases
   BOOST_CHECK(!l.template get<PinnedDatumClassBase>().abstract());
   
} // TestBase()


void ConstTest(testing::ProviderList const& l) { TestBase(l); }
void NonConstTest(testing::ProviderList& l) {
   TestBase(l);
   
   // check that no "Acquired" is available yet
   TestElement<UncopiableDatumClass>(l, "Acquired", false);
   
   // acquire a new one
   BOOST_CHECK(
     l.acquire(std::make_unique<UncopiableDatumClass>
       ("another uncopiable"), "Acquired")
     );
   TestElement<UncopiableDatumClass>(l, "Acquired", true);
   
   // erase it
   BOOST_CHECK(l.erase<UncopiableDatumClass>("Acquired"));
   TestElement<UncopiableDatumClass>(l, "Acquired", false);
   
   // erase it again
   BOOST_CHECK(!l.erase<UncopiableDatumClass>("Acquired"));
   TestElement<UncopiableDatumClass>(l, "Acquired", false);
     
   // erase something that was never there
   BOOST_CHECK(!l.erase<UncopiableDatumClass>("Never"));
   TestElement<UncopiableDatumClass>(l, "Never", false);
     
} // NonConstTest()


//------------------------------------------------------------------------------
// tests
//
BOOST_AUTO_TEST_CASE(ProviderListTest)
{
   BOOST_TEST_MESSAGE("Construction and instantiation of ProviderList");
   auto l = std::make_unique<testing::ProviderList>();
   
   BOOST_CHECK(l->setup<NormalDatumClass>(12));
   BOOST_CHECK(l->setup_instance<UncopiableDatumClass>("One", "uncopiable"));
   BOOST_CHECK(l->setup<UnmovableDatumClass>());
   BOOST_CHECK
     (l->custom_setup<PinnedDatumClass>(&PinnedDatumClass::New, 1.0, 0.0));
   BOOST_CHECK((l->set_alias<PinnedDatumClass, PinnedDatumClassBase>()));
   
   // second creation shoudl fail
   BOOST_CHECK(!l->setup_instance<UncopiableDatumClass>("One", "uncopiableII"));
   
   BOOST_TEST_MESSAGE("Constant list test");
   ConstTest(*l);
   BOOST_TEST_MESSAGE("Mutable list test");
   NonConstTest(*l);
   
   BOOST_TEST_MESSAGE("ProviderListTest completed");   
   
   // make sure that we did not leak anything
   l.reset();
   BOOST_CHECK(TrackedMemory.empty());
   
   // check that we noticed a single call for a custom setup
   BOOST_CHECK_EQUAL(PinnedDatumClass::nNewCalls, 1U);
   
} // BOOST_AUTO_TEST_CASE(ProviderListTest)
