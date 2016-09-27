#pragma once
#include "AppIncludes.h"
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/Sprite.h>
#include <Urho3D/UI/BorderImage.h>
#include "ColorDefs.h"

using namespace Urho3D;

class URHO3D_API PopUp : public Button
{
	URHO3D_OBJECT(PopUp, Button)
public:
	PopUp(Context* context);
	~PopUp() {};

	virtual void CreateUI();

	IntVector2 contentSize = IntVector2(150, 250);
	IntVector2 handleSize = IntVector2(16, 16);
	UIElement* trackedElement = NULL;

	void UpdateOrientation();
	void AddItem(UIElement* item);
	UIElement* AddItemAuto(String name);
	void RemoveItem(UIElement* item);
	void SetContentSize(int x, int y);
	void SetTrackedElement(UIElement* element);
	IntVector2 GetContetSize() { IntVector2 size; (contentRegion) ? (size = contentRegion->GetSize()) : (size = IntVector2(0, 0)); return size; }
	void UpdateLayout();

	Button* contentRegion;
	Button* handle;
	Button* canvas;
	ScrollView* canvasScroll;

	void HandleItemDelete(StringHash eventType, VariantMap& eventData);
	void HandleTrackedElementMove(StringHash eventType, VariantMap& eventData);
	void HandleElementRemoved(StringHash eventType, VariantMap& eventData);

};