#include "game/requester.h"

#include "game/text.h"

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

    for (int32_t i = 0; i < 24; i++) {
        Text_Remove(req->item_texts1[i]);
        req->item_texts1[i] = NULL;
        Text_Remove(req->item_texts2[i]);
        req->item_texts2[i] = NULL;
    }
}

void __cdecl Requester_Shutdown(REQUEST_INFO *const req)
{
    Requester_ClearTextStrings(req);
}
