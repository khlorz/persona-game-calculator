#pragma once

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <string>
#include <vector>

using StrVec = std::vector<std::string>;

namespace ImGui
{
struct ComboFilterState
{
	int  activeIdx;
	bool selectionChanged;
};

class ComboFilterCallback
{
	public:
		virtual bool ComboFilterShouldOpenPopupCallback(const char* label, char* buffer, int bufferlen,
			const StrVec& hints, int num_hints, ImGui::ComboFilterState* s);
};

void Separator(const ImVec2& spacing);

// Combo Filter widget
bool ComboFilter(const char* label, char* buffer, int bufferlen, const StrVec& hints, const int& num_hints, ComboFilterState& s, ComboFilterCallback* callback, ImGuiComboFlags flags = 0);

// For centering the image horizontally in a window
void ImageCenter(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

} // ImGui namespace

// Helper for ComboFilter
// Initialize with a vector of string and/or it's label
struct ComboFilterFunc
{
	StrVec                   list;
	int                      size;
	char                     preview[100];
	const char*              label;
	ImGuiComboFlags          flags;
	ImGui::ComboFilterState  s;

	// Initialize the Combo Filter
	ComboFilterFunc() = default;
	ComboFilterFunc(const char* _label, ImGuiComboFlags _flags = 0);
	ComboFilterFunc(const StrVec& _list, const char* _label, ImGuiComboFlags _flags = 0);
		
	// Reset the list to initial state with the new _list
	void Reset();
	void Reset(const StrVec& _list);
	
	// Get the char of the combo from the preview
	const char* GetPreview();
	
	// Get the id of the chosen state
	int GetID() const;

	// Combo with filter
	// buf = output variable
	// Returns true if you choose anything from the combo box / list
	bool Draw(float w = 0.0f);

private:
	void InitializePreview();

};

// Use only when you want to reference a certain vector for the combo list and not make another copy.
// Useful when the referenced vector has the same lifetime as the object. The vector can't be modified.
// No new operator is used and only referencing so there are no memory leaks.
struct ComboFilterPtr
{
	int                       size;
	char                      preview[100];
	const char*               label;
	ImGuiComboFlags           flags;
	ImGui::ComboFilterState   s;
	const StrVec*             list;

	ComboFilterPtr();
	ComboFilterPtr(const char* _label, ImGuiComboFlags _flags = 0);
	ComboFilterPtr(const StrVec* _list, const char* _label, ImGuiComboFlags _flags = 0);
	
	void        Reset(const StrVec* _list);
	bool        Draw(float w = 0.0f);

	int         GetID() const;
	const char* GetPreview();

private:
	void InitializePreview();
};
