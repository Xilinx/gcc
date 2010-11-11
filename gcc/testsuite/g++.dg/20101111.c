/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

template<class T> class shared_ptr
{
public:
  shared_ptr( T * p ) { }
};

class BuildingCompletedEvent
{
public:
  __attribute__((transaction_callable)) void updateBuildingSite(void);
};

void
BuildingCompletedEvent::updateBuildingSite()
{
  shared_ptr<BuildingCompletedEvent> event(new BuildingCompletedEvent());
}
