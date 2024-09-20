#include "game/ui/controls_dialog.h"

#include "game/ui/controls_column.h"
#include "game/ui/controls_layout_selector.h"
#include "game/ui/stack.h"
#include "game/ui/window.h"

#include <libtrx/memory.h>

typedef struct {
    UI_WIDGET_VTABLE vtable;
    UI_CONTROLS_CONTROLLER *controller;
    UI_WIDGET *window;
    UI_WIDGET *layout_selector;
    UI_WIDGET *outer_stack;
    UI_WIDGET *column_stack;
    UI_WIDGET *left_column;
    UI_WIDGET *right_column;
} UI_CONTROLS_DIALOG;

static int32_t M_GetWidth(const UI_CONTROLS_DIALOG *self);
static int32_t M_GetHeight(const UI_CONTROLS_DIALOG *self);
static void M_SetPosition(UI_CONTROLS_DIALOG *self, int32_t x, int32_t y);
static void M_Control(UI_CONTROLS_DIALOG *self);
static void M_Free(UI_CONTROLS_DIALOG *self);

static int32_t M_GetWidth(const UI_CONTROLS_DIALOG *const self)
{
    return self->window->get_width(self->window);
}

static int32_t M_GetHeight(const UI_CONTROLS_DIALOG *const self)
{
    return self->window->get_height(self->window);
}

static void M_SetPosition(
    UI_CONTROLS_DIALOG *const self, const int32_t x, const int32_t y)
{
    return self->window->set_position(self->window, x, y);
}

static void M_Control(UI_CONTROLS_DIALOG *const self)
{
    if (UI_ControlsController_Control(self->controller)) {
        // Trigger the UI updates only if anything has changed.
        self->window->control(self->window);
        // Reposition the header.
        UI_Stack_DoLayout(self->outer_stack);
    }
}

static void M_Free(UI_CONTROLS_DIALOG *const self)
{
    self->left_column->free(self->left_column);
    self->right_column->free(self->right_column);
    self->column_stack->free(self->column_stack);
    self->outer_stack->free(self->outer_stack);
    self->layout_selector->free(self->layout_selector);
    self->window->free(self->window);
    Memory_Free(self);
}

UI_WIDGET *UI_ControlsDialog_Create(UI_CONTROLS_CONTROLLER *const controller)
{
    UI_CONTROLS_DIALOG *const self = Memory_Alloc(sizeof(UI_CONTROLS_DIALOG));
    self->vtable = (UI_WIDGET_VTABLE) {
        .control = (UI_WIDGET_CONTROL)M_Control,
        .get_width = (UI_WIDGET_GET_WIDTH)M_GetWidth,
        .get_height = (UI_WIDGET_GET_HEIGHT)M_GetHeight,
        .set_position = (UI_WIDGET_SET_POSITION)M_SetPosition,
        .free = (UI_WIDGET_FREE)M_Free,
    };

    self->controller = controller;
    self->controller->state = UI_CONTROLS_STATE_NAVIGATE_LAYOUT;

    self->layout_selector = UI_ControlsLayoutSelector_Create(self->controller);
    self->left_column = UI_ControlsColumn_Create(0, self->controller);
    self->right_column = UI_ControlsColumn_Create(1, self->controller);

    self->column_stack = UI_Stack_Create(UI_STACK_LAYOUT_HORIZONTAL);
    UI_Stack_AddChild(self->column_stack, self->left_column);
    UI_Stack_AddChild(self->column_stack, self->right_column);

    self->outer_stack = UI_Stack_Create(UI_STACK_LAYOUT_VERTICAL);
    UI_Stack_AddChild(self->outer_stack, self->layout_selector);
    UI_Stack_AddChild(self->outer_stack, self->column_stack);

    self->window = UI_Window_Create(self->outer_stack, 5, 5, 15, 5);

    return (UI_WIDGET *)self;
}
