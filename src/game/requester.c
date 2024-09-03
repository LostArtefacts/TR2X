#include "game/requester.h"

#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

static void Requester_ClearTextStrings(REQUEST_INFO *req);

static void Requester_ClearTextStrings(REQUEST_INFO *const req)
{
    Text_Remove(req->heading_text1);
    req->heading_text1 = NULL;

    Text_Remove(req->heading_text2);
    req->heading_text2 = NULL;

    Text_Remove(req->background_text);
    req->background_text = NULL;

    Text_Remove(req->moreup_text);
    req->moreup_text = NULL;

    Text_Remove(req->moredown_text);
    req->moredown_text = NULL;

    for (int32_t i = 0; i < MAX_REQUESTER_ITEMS; i++) {
        Text_Remove(req->item_texts1[i]);
        req->item_texts1[i] = NULL;
        Text_Remove(req->item_texts2[i]);
        req->item_texts2[i] = NULL;
    }
}

void __cdecl Requester_Init(REQUEST_INFO *const req)
{
    req->background_flags = 1;
    req->moreup_flags = 1;
    req->moredown_flags = 1;
    req->items_count = 0;

    req->heading_text1 = NULL;
    req->heading_text2 = NULL;
    req->heading_flags1 = 0;
    req->heading_flags2 = 0;
    req->background_text = NULL;
    req->moreup_text = NULL;
    req->moredown_text = NULL;

    for (int32_t i = 0; i < MAX_REQUESTER_ITEMS; i++) {
        req->item_texts1[i] = NULL;
        req->item_texts2[i] = NULL;
        req->item_flags1[i] = 0;
        req->item_flags2[i] = 0;
    }

    req->pitem_flags1 = g_RequesterFlags1;
    req->pitem_flags2 = g_RequesterFlags2;

    req->render_width = GetRenderWidth();
    req->render_height = GetRenderHeight();
}

void __cdecl Requester_Shutdown(REQUEST_INFO *const req)
{
    Requester_ClearTextStrings(req);
}
