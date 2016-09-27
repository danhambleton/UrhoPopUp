#include "PopUp.h"
#include "Urho3D/Graphics/Texture2D.h"

PopUp::PopUp(Context* context) :Button(context)
{

}

void PopUp::CreateUI()
{
	ResourceCache* rc = GetSubsystem<ResourceCache>();

	contentRegion = new Button(GetContext());
	contentRegion->SetSize(contentSize);
	contentRegion->SetTexture(rc->GetResource<Texture2D>("Textures/PopupContent.png"));
	contentRegion->SetImageRect(IntRect(0, 0, 32, 32));
	contentRegion->SetBorder(IntRect(2, 2, 8, 8));
	contentRegion->SetBlendMode(BlendMode::BLEND_ALPHA);
	contentRegion->SetColor(Color::WHITE);
	AddChild(contentRegion);

	handle = new Button(GetContext());
	handle->SetSize(handleSize);
	handle->SetTexture(rc->GetResource<Texture2D>("Textures/PopupHandle.png"));
	handle->SetImageRect(IntRect(0, 0, 32, 32));
	handle->SetBlendMode(BlendMode::BLEND_ALPHA);
	handle->SetColor(Color::WHITE);
	AddChild(handle);

	//create the scroll view

	//set up the canvas scroll
	canvas = new Button(GetContext());
	canvas->SetName("GraphCanvas");
	canvas->SetSize(contentSize.x_-12, 32);
	canvas->SetPosition(2, 2);
	canvas->SetUseDerivedOpacity(false);
	canvas->SetOpacity(0.0f);
	canvas->SetColor(MIDDLE_GRAY);

	canvasScroll = new ScrollView(GetContext());
	canvasScroll->SetName("GraphScrollView");
	canvasScroll->SetSize(contentSize.x_ - 12, contentSize.y_-12);
	canvasScroll->SetPosition(2,2);
	canvasScroll->SetUseDerivedOpacity(false);
	canvasScroll->SetStyleAuto();
	canvasScroll->SetColor(MIDDLE_GRAY);
	canvasScroll->SetOpacity(0.1f);

	//canvasScroll->SetVisible(false);
	//canvasScroll->SetStyleAuto();
	contentRegion->AddChild(canvasScroll);
	canvasScroll->AddChild(canvas);

	canvasScroll->SetContentElement(canvas);
	canvasScroll->SetScrollBarsVisible(false, true);
	canvasScroll->SetViewPosition(0, 0);

	UpdateOrientation();

	SubscribeToEvent("LayoutChanged", URHO3D_HANDLER(PopUp, HandleElementRemoved));
}

void PopUp::AddItem(UIElement* item)
{

}

UIElement* PopUp::AddItemAuto(String name)
{
	UIElement* content = canvasScroll->GetContentElement();
	int numItems = content->GetNumChildren();

	ResourceCache* rc = GetSubsystem<ResourceCache>();

	//set the container
	UIElement* lineItem = new UIElement(GetContext());
	lineItem->SetPosition(2, numItems * 32);
	lineItem->SetSize(canvasScroll->GetSize().x_ - 4, 30);
	lineItem->SetName(name);
	content->AddChild(lineItem);

	//add the delete button
	Button* dButton = new Button(GetContext());
	dButton->SetPosition(0, 8);
	dButton->SetSize(16, 16);
	dButton->SetTexture(rc->GetResource<Texture2D>("Textures/close_circle.png"));
	dButton->SetColor(BRIGHT_ORANGE);
	dButton->SetBlendMode(BLEND_ALPHA);
	dButton->SetUseDerivedOpacity(false);
	dButton->SetName("delete");
	lineItem->AddChild(dButton);

	//SubscribeToEvent(dButton, E_PRESSED, URHO3D_HANDLER(PopUp, HandleItemDelete));


	//create the view button
	Button* item = new Button(canvasScroll->GetContext());
	item->SetPosition(18, 0);
	item->SetSize(canvasScroll->GetSize().x_ - 18, 30);
	item->SetColor(Color(1,1,1,0.1f));
	item->SetUseDerivedOpacity(false);
	item->SetOpacity(0.7f);
	lineItem->AddChild(item);
	item->SetName(name); //track the name of the view

						 //create the text to annotate
	Text* txt = new Text(GetContext());
	txt->SetStyle("SmallLabelText");
	txt->SetText(name);
	txt->SetPosition(5, 5);
	txt->SetUseDerivedOpacity(false);
	item->AddChild(txt);

	//update canvas size
	canvas->SetSize(canvas->GetSize().x_, canvas->GetSize().y_ + 32);


	return lineItem;
}

void PopUp::HandleItemDelete(StringHash eventType, VariantMap& eventData)
{
	using namespace Pressed;
	Button* b = (Button*)eventData[P_ELEMENT].GetVoidPtr();
	if (b)
	{
		UIElement* parent = b->GetParent();
		if (parent)
		{
			RemoveItem(parent);
		}
	}
}

void PopUp::HandleElementRemoved(StringHash eventType, VariantMap& eventData)
{
	UpdateLayout();
}

void PopUp::RemoveItem(UIElement* item)
{
	item->Remove();

	//redo layout
	UpdateLayout();
}

void PopUp::UpdateLayout()
{
	UIElement* content = canvasScroll->GetContentElement();
	int numItems = content->GetNumChildren();

	for (int i = 0; i < numItems; i++)
	{
		UIElement* item = content->GetChild(i);
		item->SetPosition(item->GetPosition().x_, i * 32);
	}
}

void PopUp::SetContentSize(int x, int y)
{
	if (!contentRegion)
		return;

	//update contentRegion size
	contentSize = IntVector2(x, y);
	contentRegion->SetSize(x, y);

	//update canvas and scroll
	canvas->SetSize(x - 12, canvas->GetSize().y_);
	canvasScroll->SetSize(x - 12, y - 12);

	//update all child elements
	PODVector<UIElement*> children;
	canvas->GetChildren(children, false);

	for (int i = 0; i < children.Size(); i++)
	{
		children[i]->SetSize(canvasScroll->GetSize().x_ - 18, 30);
	}

	UpdateOrientation();
}


void PopUp::UpdateOrientation()
{
	//layout varies depending on the parent's position and size

	//TODO: handle other layouts

	//first determine the type of layout
	//1 - handle on right, 
	//2 - handle on left,
	//3 - handle on top,
	//4 - handle on bottom

	int layoutType = 1;
	UI* ui = GetSubsystem<UI>();

	int pX = ui->GetRoot()->GetSize().x_;
	int pY = ui->GetRoot()->GetSize().y_;
	int tX = GetScreenPosition().x_;
	int tY = GetScreenPosition().y_;

	int margin = 16;

	if (tX < contentSize.x_ && tY < pY - margin)
		layoutType = 2;
	if (tX > contentSize.x_ && tY < pY - margin)
		layoutType = 1;
	if (tY >= pY - margin)
		layoutType = 4;
	if (tY <= margin)
		layoutType = 3;

	//adjust layout accordingly
	int cX, cY, hX, hY;
	switch (layoutType)
	{
	case 1:
		//layout content to the right of the position
		cX = -contentSize.x_ - handleSize.x_;
		cY = -contentSize.y_ / 2;
		if (tY > pY - contentSize.y_ / 2 - 10)
			cY = -(contentSize.y_ - (pY - tY - 10));
		contentRegion->SetPosition(cX, cY);

		//layout handle
		hX = -handleSize.x_-8;
		hY = -handleSize.y_ / 2;
		handle->SetImageRect(IntRect(0, 0, 32, 32));
		handle->SetPosition(hX, hY);
		break;

	case 2:
		//layout content to the right of the position
		cX = handleSize.x_;
		cY = -contentSize.y_ / 2;
		cY = Min(cY, pY - contentSize.y_ + tY);
		contentRegion->SetPosition(cX, cY);

		//layout handle
		hX = 0;
		hY = handleSize.y_ / 2;
		handle->SetImageRect(IntRect(32, 0, 64, 32));
		handle->SetPosition(hX, hY);
		break;

	case 3:

		//layout content to the right of the position
		cX = -contentSize.x_ / 2;
		cY = handleSize.y_;
		//cY = Min(cY, pY - contentSize.y_ + tY);
		contentRegion->SetPosition(cX, cY);

		//layout handle
		hX = -handleSize.x_ / 2;
		hY = 0;
		handle->SetImageRect(IntRect(64, 0, 96, 32));
		handle->SetPosition(hX, hY);	
		break;


	case 4:
		//layout content to the right of the position
		cX = -contentSize.x_/2;
		cY = -contentSize.y_ - handleSize.y_;
		//cY = Min(cY, pY - contentSize.y_ + tY);
		contentRegion->SetPosition(cX, cY);

		//layout handle
		hX = -handleSize.x_ / 2;
		hY = -handleSize.y_ - 8;
		handle->SetImageRect(IntRect(96, 0, 128, 32));
		handle->SetPosition(hX, hY);

		break;
	}

}

void PopUp::SetTrackedElement(UIElement* element)
{
	if (trackedElement)
	{
		UnsubscribeFromEvent(E_POSITIONED);
	}
	
	trackedElement = element;
	IntVector2 pos = trackedElement->GetScreenPosition();
	pos.y_ += trackedElement->GetSize().y_ / 2;
	SetPosition(pos);
	SubscribeToEvent(trackedElement, E_POSITIONED, URHO3D_HANDLER(PopUp, HandleTrackedElementMove));
	UpdateOrientation();
}

void PopUp::HandleTrackedElementMove(StringHash eventType, VariantMap& eventData)
{
	using namespace Positioned;

	if (trackedElement)
	{
		IntVector2 pos = trackedElement->GetScreenPosition();
		pos.y_ += trackedElement->GetSize().y_ / 2;
		SetPosition(pos);
		UpdateOrientation();
	}
}