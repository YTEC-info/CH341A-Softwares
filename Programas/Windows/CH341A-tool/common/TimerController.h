/** \file
    Small utility class to temporarily disable timer and reenable it if it was active on entry
*/

#ifndef TimerControllerH
#define TimerControllerH

#include <assert.h>

class TimerController {
private:
    TTimer *tmr;
	bool enabled;
public:
    TimerController(TTimer *tmr): tmr(tmr) {
        assert(tmr);
		enabled = tmr->Enabled;
        tmr->Enabled = false;
    }
    ~TimerController(void) {
		if (enabled) {
			tmr->Enabled = true;
		}
    }
};

#endif