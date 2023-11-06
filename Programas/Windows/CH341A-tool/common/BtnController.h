/** \file
    Small utility class to drive button state blocking reentrancy
*/

#ifndef BtnControllerH
#define BtnControllerH

#include <assert.h>

class BtnController {
private:
    TButton *btn;
public:
    BtnController(TButton *btn): btn(btn) {
        assert(btn);
        btn->Enabled = false;
    }
    ~BtnController(void) {
        btn->Enabled = true;
    }
};

#endif