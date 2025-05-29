#pragma once
#include "utils/common.h"
#include "graphics/event.hpp"

#if !defined (CYX_POINTER_BASED_APPLICATION)
    #define CYX_INHERITANCE_BASED_APPLICATION 
#endif

#if defined (CYX_POINTER_BASED_APPLICATION)
    #undef CYX_INHERITANCE_BASED_APPLICATION
#endif

namespace cyx {

    class Window;

#if defined (CYX_POINTER_BASED_APPLICATION)
	struct Application {
        Application();
		void (*on_init)(Window&) = nullptr;
		void (*on_create)(Window&)  = nullptr;
		void (*on_update)(Window&,f64 dt)  = nullptr;;
		void (*on_event) (Window&,Event e) = nullptr;;
		void (*on_destroy)(Window&) = nullptr;;
	};
#else
	struct Application {
        Application();
		virtual void on_init(Window&);
		virtual void on_create(Window&) ;
		virtual void on_update(Window&,f64 dt) ;
		virtual void on_event (Window&,Event e);
		virtual void on_destroy(Window&);
	};

#endif

}
