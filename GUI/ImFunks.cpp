#include "ImFunks.h"
#include "imgui_internal.h"

ComboFilterFunc::ComboFilterFunc(const StrVec& _list, const char* _label, ImGuiComboFlags _flags) :
	list(_list), 
	label(_label), 
	size(_list.size()), 
	s({0, false}), 
	flags(_flags)
{
	InitializePreview();
}

ComboFilterFunc::ComboFilterFunc(const char* _label, ImGuiComboFlags _flags) :
	list(StrVec()),
	label(_label),
	size(0),
	s({ 0, false }),
	flags(_flags)
{
	InitializePreview();
}

void ComboFilterFunc::InitializePreview()
{
	if (list.empty())
		memcpy(preview, "N/A", strlen("N/A") + 1);
	else
		memcpy(preview, list[0].c_str(), strlen(list[0].c_str()) + 1);
}

void ComboFilterFunc::Reset(const StrVec& _list)
{
	list        = _list;
	size        = list.size();
	s           = { 0, false };

	InitializePreview();
}

void ComboFilterFunc::Reset()
{
	s           = { 0, false };

	InitializePreview();
}

int ComboFilterFunc::GetID() const
{
	return s.activeIdx;
}

const char* ComboFilterFunc::GetPreview()
{
	return preview;
}

bool ComboFilterFunc::Draw(float w)
{
	// The Main Combo Filter
	if (w != 0.0f)
		ImGui::PushItemWidth(w);

	if (list.empty())
	{
		ImGui::PushDisabled(true);
		static const std::vector<std::string> empty_list = { "N/A" };
		ImGui::ComboFilter(label, preview, 100, empty_list, 1, s, NULL, flags);
		ImGui::PopDisabled();

		return false;
	}

	return ImGui::ComboFilter(label, preview, 100, list, size, s, NULL, flags);
}


//--------------------------------------------------------
// COMBO FILTER POINTER LIST
//	For list that doesn't get changed or constant,
//	no copies are made from the list
//--------------------------------------------------------

ComboFilterPtr::ComboFilterPtr() :
	list(nullptr),
	size(0),
	s({ 0, false })
{	
	InitializePreview();
}

ComboFilterPtr::ComboFilterPtr(const StrVec* _list, const char* _label, ImGuiComboFlags _flags) :
	list(_list), 
	label(_label), 
	size(list->size()), 
	s({0, false}), 
	flags(_flags)
{
	InitializePreview();
}

ComboFilterPtr::ComboFilterPtr(const char* _label, ImGuiComboFlags _flags) :
	list(nullptr), 
	label(_label), 
	size(0), 
	s({0, false}), 
	flags(_flags)
{
	InitializePreview();
}

void ComboFilterPtr::Reset(const StrVec* _list)
{
	list = _list;
	size = list->size();
	s    = { 0, false };
	InitializePreview();
}

bool ComboFilterPtr::Draw(float w)
{
	// The Main Combo Filter
	if (w != 0.0f)
		ImGui::PushItemWidth(w);

	if (list->empty())
	{
		ImGui::PushDisabled(true);
		static const std::vector<std::string> empty_list = { "N/A" };
		ImGui::ComboFilter(label, preview, 100, empty_list, 1, s, NULL, flags);
		ImGui::PopDisabled();

		return false;
	}

	return ImGui::ComboFilter(label, preview, 100, *list, size, s, NULL, flags);
}

int ComboFilterPtr::GetID() const
{
	return s.activeIdx;
}

const char* ComboFilterPtr::GetPreview()
{
	return preview;
}

void ComboFilterPtr::InitializePreview()
{
	if (list == nullptr || list->empty())
		memcpy(preview, "N/A", strlen("N/A") + 1);
	else
		memcpy(preview, (*list)[0].c_str(), strlen((*list)[0].c_str()) + 1);
}




//----------------------------------------------------
// IMGUI TEXT FILTER ADDITIONAL FILTER
//----------------------------------------------------

bool ImGuiTextFilter::DrawWithHint(const char* label, float width)
{
	if (width != 0.0f)
		ImGui::SetNextItemWidth(width);
	std::string identifier = std::string("##Input_") += label;
	bool value_changed = ImGui::InputTextWithHint(identifier.c_str(), label, InputBuf, IM_ARRAYSIZE(InputBuf));
	if (value_changed)
		Build();
	return value_changed;
}

//--------------------------------------------------------------------------
// ADDITIONAL IMGUI FUNCTIONS
//--------------------------------------------------------------------------
// All functions that are added to the ImGui namespace are here
// 1.) ComboFilter
// 2.) Center Image display
//

namespace ImGui
{
//-----------------------------------------------------
// COMBO FILTER
//-----------------------------------------------------

void Separator(const ImVec2& spacing)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);
	ImGui::Separator();
	ImGui::PopStyleVar();
}

// ------------------------------------------------------------
// COMBO FILTER WIDGET!
// ------------------------------------------------------------
// fuzzy-completion combobox code by kovewnikov, from: https://github.com/ocornut/imgui/issues/1658
// corrections and bug fixes by Marcin "Slajerek" Skoczylas

bool ComboFilter(const char* label, char* buffer, int bufferlen, const StrVec& hints, const int& num_hints, ComboFilterState& s, ComboFilterCallback* callback, ImGuiComboFlags flags)
{
	s.selectionChanged = false;

	// Always consume the SetNextWindowSizeConstraint() call in our early return paths
	ImGuiContext& g = *GImGui;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiID popupId = window->GetID(label);
	bool popupIsAlreadyOpened = IsPopupOpen(popupId, 0); //ImGuiPopupFlags_AnyPopupLevel);
	bool popupNeedsToBeOpened = callback ? callback->ComboFilterShouldOpenPopupCallback(label, buffer, bufferlen, hints, num_hints, &s)
		: (buffer[0] != 0) && strcmp(buffer, hints[s.activeIdx].c_str());
	bool popupJustOpened = false;

	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

	const ImGuiStyle& style = g.Style;

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const float expected_w = CalcItemWidth();
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
	const ImRect frame_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + w, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, ImVec2((label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f) + frame_bb.Max.x, frame_bb.Max.y));
	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, popupId, &frame_bb))
		return false;


	bool hovered, held;
	bool pressed = ButtonBehavior(frame_bb, popupId, &hovered, &held);

	if (!popupIsAlreadyOpened) {
		const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderNavHighlight(frame_bb, popupId);
		if (!(flags & ImGuiComboFlags_NoPreview))
			window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersLeft);
	}
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		ImU32 bg_col = GetColorU32((popupIsAlreadyOpened || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImU32 text_col = GetColorU32(ImGuiCol_Text);
		window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
		if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
	}

	if (!popupIsAlreadyOpened)
	{
		RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
		if (buffer != NULL && !(flags & ImGuiComboFlags_NoPreview))

			RenderTextClipped(ImVec2(frame_bb.Min.x + style.FramePadding.x, frame_bb.Min.y + style.FramePadding.y), ImVec2(value_x2, frame_bb.Max.y), buffer, NULL, NULL, ImVec2(0.0f, 0.0f));

		if ((pressed || g.NavActivateId == popupId || popupNeedsToBeOpened) && !popupIsAlreadyOpened)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = popupId;
			OpenPopupEx(popupId);
			popupIsAlreadyOpened = true;
			popupJustOpened = true;
		}
	}

	if (label_size.x > 0)
	{
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);
	}

	if (!popupIsAlreadyOpened) {
		return false;
	}

	const float totalWMinusArrow = w - arrow_size;
	struct ImGuiSizeCallbackWrapper {
		static void sizeCallback(ImGuiSizeCallbackData* data)
		{
			float* totalWMinusArrow = (float*)(data->UserData);
			data->DesiredSize = ImVec2(*totalWMinusArrow, 200.f);
		}
	};
	SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(totalWMinusArrow, 150.f), ImGuiSizeCallbackWrapper::sizeCallback, (void*)&totalWMinusArrow);

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	// Peek into expected window size so we can position it
	if (ImGuiWindow* popup_window = FindWindowByName(name))
	{
		if (popup_window->WasActive)
		{
			ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
			if (flags & ImGuiComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = ImGuiDir_Left;
			ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);

			pos.y -= label_size.y + style.FramePadding.y * 2.0f;

			SetNextWindowPos(pos);
		}
	}

	// Horizontally align ourselves with the framed text
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
	//    PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = Begin(name, NULL, window_flags);

	ImGui::PushItemWidth(ImGui::GetWindowWidth());
	ImGui::SetCursorPos(ImVec2(0.f, window->DC.CurrLineTextBaseOffset));
	if (popupJustOpened)
	{
		ImGui::SetKeyboardFocusHere(0);
	}

	bool done = InputTextEx("##inputText", NULL, buffer, bufferlen, ImVec2(0, 0),
		ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue,
		NULL, NULL);
	ImGui::PopItemWidth();

	if (s.activeIdx < 0) {
		IM_ASSERT(false); //Undefined behaviour
		return false;
	}


	if (!ret)
	{
		ImGui::EndChild();
		ImGui::PopItemWidth();
		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}


	ImGuiWindowFlags window_flags2 = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus; //0; //ImGuiWindowFlags_HorizontalScrollbar
	ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false, window_flags2);

	struct fuzzy {
		static int score(const char* str1, const char* str2) {
			int score = 0, consecutive = 0, maxerrors = 0;
			while (*str1 && *str2) {
				int is_leading = (*str1 & 64) && !(str1[1] & 64);
				if ((*str1 & ~32) == (*str2 & ~32)) {
					int had_separator = (str1[-1] <= 32);
					int x = had_separator || is_leading ? 10 : consecutive * 5;
					consecutive = 1;
					score += x;
					++str2;
				}
				else {
					int x = -1, y = is_leading * -3;
					consecutive = 0;
					score += x;
					maxerrors += y;
				}
				++str1;
			}
			return score + (maxerrors < -9 ? -9 : maxerrors);
		}
		static int search(const char* str, int num, const std::vector<std::string>& words) {
			int scoremax = 0;
			int best = -1;
			for (int i = 0; i < num; ++i) {
				int score = fuzzy::score(words[i].c_str(), str);
				int record = (score >= scoremax);
				int draw = (score == scoremax);
				if (record) {
					scoremax = score;
					if (!draw) best = i;
					else best = best >= 0 && strlen(words[best].c_str()) < strlen(words[i].c_str()) ? best : i;
				}
			}
			return best;
		}
	};

	bool selectionChangedLocal = false;
	if (buffer[0] != '\0')
	{
		int new_idx = fuzzy::search(buffer, num_hints, hints);
		int idx = new_idx >= 0 ? new_idx : s.activeIdx;
		s.selectionChanged = s.activeIdx != idx;
		selectionChangedLocal = s.selectionChanged;
		s.activeIdx = idx;
	}

	bool arrowScroll = false;
	int arrowScrollIdx = s.activeIdx;

	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)))
	{
		if (s.activeIdx > 0)
		{
			s.activeIdx--;
			//			selectionChangedLocal = true;
			arrowScroll = true;
			ImGui::SetWindowFocus();
			//			arrowScrollIdx = s.activeIdx;
		}
	}
	if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)))
	{
		if (s.activeIdx < num_hints - 1)
		{
			s.activeIdx++;
			//			selectionChangedLocal = true;
			arrowScroll = true;
			ImGui::SetWindowFocus();
			//			arrowScrollIdx = s.activeIdx;
		}
	}

	if (done && !arrowScroll) {
		CloseCurrentPopup();
	}

	bool done2 = false;

	for (int n = 0; n < num_hints; n++)
	{
		bool is_selected = n == s.activeIdx;
		if (is_selected && (IsWindowAppearing() || selectionChangedLocal))
		{
			SetScrollHereY();
			//            ImGui::SetItemDefaultFocus();
		}

		if (is_selected && arrowScroll)
		{
			SetScrollHereY();
		}

		if (ImGui::Selectable(hints[n].c_str(), is_selected))
		{
			s.selectionChanged = s.activeIdx != n;
			s.activeIdx = n;
			strcpy(buffer, hints[n].c_str());
			CloseCurrentPopup();

			done2 = true;
		}

	}

	if (arrowScroll)
	{
		strcpy(buffer, hints[s.activeIdx].c_str());
		ImGuiWindow* window = FindWindowByName(name);
		const ImGuiID id = window->GetID("##inputText");
		ImGuiInputTextState* state = GetInputTextState(id);

		const char* buf_end = NULL;
		state->CurLenW = ImTextStrFromUtf8(state->TextW.Data, state->TextW.Size, buffer, NULL, &buf_end);
		state->CurLenA = (int)(buf_end - buffer);
		state->CursorClamp();
	}

	ImGui::EndChild();
	EndPopup();

	bool ret1 = (s.selectionChanged && !strcmp(hints[s.activeIdx].c_str(), buffer));

	bool widgetRet = done || done2 || ret1;

	return widgetRet;
}

bool ImGui::ComboFilterCallback::ComboFilterShouldOpenPopupCallback(const char* label, char* buffer, int bufferlen, const StrVec& hints, int num_hints, ImGui::ComboFilterState* s)
{
	return (buffer[0] != 0) && strcmp(buffer, hints[s->activeIdx].c_str());
}

//-----------------------------------------------------
// CENTER IMAGE DISPLAY
//-----------------------------------------------------

void ImGui::ImageCenter(ImTextureID user_texture_id, const ImVec2& imgsize, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImRect bb;
	bb.Min = ImVec2(window->DC.CursorPos.x + ((window->Size.x - imgsize.x) / 2.0f), window->DC.CursorPos.y);
	bb.Max = ImVec2(imgsize.x + bb.Min.x, imgsize.y + bb.Min.y);
	if (border_col.w > 0.0f)
		bb.Max += ImVec2(2, 2);
	ItemSize(bb);
	if (!ItemAdd(bb, 0))
		return;

	if (border_col.w > 0.0f)
	{
		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
		window->DrawList->AddImage(user_texture_id, bb.Min + ImVec2(1, 1), bb.Max - ImVec2(1, 1), uv0, uv1, GetColorU32(tint_col));
	}
	else
	{
		window->DrawList->AddImage(user_texture_id, bb.Min, bb.Max, uv0, uv1, GetColorU32(tint_col));
	}
}

} // ImGui namespace
