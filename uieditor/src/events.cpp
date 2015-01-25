#include "DocumentView.h"
#include "events.h"

extern DocumentView*    g_pDocument;
extern onut::UIContext* g_pUIContext;
extern onut::UIControl* g_pUIScreen;

onut::UICheckBox*    g_pInspector_UIControl_chkEnabled;
onut::UICheckBox*    g_pInspector_UIControl_chkVisible;
onut::UICheckBox*    g_pInspector_UIControl_chkClickThrough;
onut::UIButton*      g_pInspector_UIControl_txtName;
onut::UIButton*      g_pInspector_UIControl_txtStyle;
onut::UIButton*      g_pInspector_UIControl_txtX;
onut::UIButton*      g_pInspector_UIControl_txtY;
onut::UICheckBox*    g_pInspector_UIControl_chkXPercent;
onut::UICheckBox*    g_pInspector_UIControl_chkYPercent;
onut::UIButton*      g_pInspector_UIControl_txtWidth;
onut::UIButton*      g_pInspector_UIControl_txtHeight;
onut::UICheckBox*    g_pInspector_UIControl_chkWidthPercent;
onut::UICheckBox*    g_pInspector_UIControl_chkHeightPercent;
onut::UICheckBox*    g_pInspector_UIControl_chkWidthRelative;
onut::UICheckBox*    g_pInspector_UIControl_chkHeightRelative;
onut::UICheckBox*    g_pInspector_UIControl_chkTOP_LEFT;
onut::UICheckBox*    g_pInspector_UIControl_chkTOP;
onut::UICheckBox*    g_pInspector_UIControl_chkTOP_RIGHT;
onut::UICheckBox*    g_pInspector_UIControl_chkLEFT;
onut::UICheckBox*    g_pInspector_UIControl_chkCENTER;
onut::UICheckBox*    g_pInspector_UIControl_chkRIGHT;
onut::UICheckBox*    g_pInspector_UIControl_chkBOTTOM_LEFT;
onut::UICheckBox*    g_pInspector_UIControl_chkBOTTOM;
onut::UICheckBox*    g_pInspector_UIControl_chkBOTTOM_RIGHT;

onut::UIControl* getCreateParent()
{
    auto pSelected = g_pDocument->pSelected;
    if (!pSelected)
    {
        return g_pDocument->pUIScreen;
    }
    else
    {
        if (OInput->isStateDown(DIK_LSHIFT))
        {
            return pSelected;
        }
        else if (pSelected->getParent())
        {
            return pSelected->getParent();
        }
        else
        {
            return g_pDocument->pUIScreen;
        }
    }
}

void onCreateControl(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto pParent = getCreateParent();
    auto pCtrl = new onut::UIControl();

    pCtrl->setRect({{0, 0}, {100, 100}});

    pParent->add(pCtrl);
    g_pDocument->controlCreated(pCtrl, pParent);
    g_pDocument->setSelected(pCtrl);
}

void onCreatePanel(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto pParent = getCreateParent();
    auto pPanel = new onut::UIPanel();

    pPanel->setRect({{0, 0}, {100, 100}});

    pParent->add(pPanel);
    g_pDocument->controlCreated(pPanel, pParent);
    g_pDocument->setSelected(pPanel);
}

void onCreateButton(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto pParent = getCreateParent();
    auto pBtn = new onut::UIButton();

    pBtn->setCaption("Button");
    pBtn->setRect({{4, 4}, {64, 24}});

    pParent->add(pBtn);
    g_pDocument->controlCreated(pBtn, pParent);
    g_pDocument->setSelected(pBtn);
}

void onCreateLabel(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto pParent = getCreateParent();
    auto pLbl = new onut::UILabel();

    pLbl->setText("Label");
    pLbl->setRect({{4, 4}, {64, 20}});

    pParent->add(pLbl);
    g_pDocument->controlCreated(pLbl, pParent);
    g_pDocument->setSelected(pLbl);
}

void onCreateImage(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto pParent = getCreateParent();
    auto pImg = new onut::UIImage();

    pImg->setRect({{4, 4}, {32, 32}});

    pParent->add(pImg);
    g_pDocument->controlCreated(pImg, pParent);
    g_pDocument->setSelected(pImg);
}

bool g_bSelected = false;

void onSelect(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    auto mousePos = evt.mousePos;
    auto rect = g_pUIScreen->getChild("pnlRegion")->getWorldRect(*g_pUIContext);
    mousePos.x -= rect.position.x;
    mousePos.y -= rect.position.y;
    auto pPickedControl = g_pDocument->pUIScreen->getChild(*g_pDocument->pUIContext, mousePos);
    g_pDocument->setSelected(pPickedControl);
    if (pPickedControl)
    {
        g_bSelected = true;
        g_pDocument->onGizmoStart(pPickedControl, evt);
    }
}

void onSelectUp(onut::UIControl* pControl, const onut::UIMouseEvent& evt)
{
    if (g_bSelected)
    {
        g_bSelected = false;
        if (g_pDocument->pSelected)
        {
            g_pDocument->onGizmoEnd(g_pDocument->pSelected, evt);
        }
    }
}

void onSceneGraphSelectionChanged(onut::UITreeView* pControl, const onut::UITreeViewSelectEvent& evt)
{
    if (evt.pSelectedItems->empty())
    {
        g_pDocument->setSelected(nullptr, false);
    }
    else
    {
        auto pViewItem = evt.pSelectedItems->front();
        auto pSelected = static_cast<onut::UIControl*>(pViewItem->getUserData());
        g_pDocument->setSelected(pSelected, false);
    }
}

void onAlignChkChanged(onut::UICheckBox* pCheckBox, const onut::UICheckEvent& evt)
{
    if (!evt.isChecked) return; // Don't care
    if (!g_pDocument->pSelected) return; // Wuuuut?
    if (pCheckBox == g_pInspector_UIControl_chkTOP_LEFT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::TOP_LEFT);
    else if (pCheckBox == g_pInspector_UIControl_chkTOP)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::TOP);
    else if (pCheckBox == g_pInspector_UIControl_chkTOP_RIGHT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::TOP_RIGHT);
    else if (pCheckBox == g_pInspector_UIControl_chkLEFT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::LEFT);
    else if (pCheckBox == g_pInspector_UIControl_chkCENTER)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::CENTER);
    else if (pCheckBox == g_pInspector_UIControl_chkRIGHT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::RIGHT);
    else if (pCheckBox == g_pInspector_UIControl_chkBOTTOM_LEFT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::BOTTOM_LEFT);
    else if (pCheckBox == g_pInspector_UIControl_chkBOTTOM)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::BOTTOM);
    else if (pCheckBox == g_pInspector_UIControl_chkBOTTOM_RIGHT)
        g_pDocument->pSelected->setAlign(onut::eUIAlign::BOTTOM_RIGHT);
}

void hookUIEvents(onut::UIControl* pUIScreen)
{
    // Tool box
    pUIScreen->getChild("btnCreateControl")->onClick = onCreateControl;
    pUIScreen->getChild("btnCreatePanel")->onClick = onCreatePanel;
    pUIScreen->getChild("btnCreateButton")->onClick = onCreateButton;
    pUIScreen->getChild("btnCreateLabel")->onClick = onCreateLabel;
    pUIScreen->getChild("btnCreateImage")->onClick = onCreateImage;

    // View
    pUIScreen->getChild("pnlView")->onMouseDown = onSelect;
    pUIScreen->getChild("pnlView")->onMouseUp = onSelectUp;
    pUIScreen->getChild("pnlRegion")->onMouseDown = onSelect;
    pUIScreen->getChild("pnlRegion")->onMouseUp = onSelectUp;

    // Scene Graph
    dynamic_cast<onut::UITreeView*>(pUIScreen->getChild("sceneGraph"))->onSelectionChanged = onSceneGraphSelectionChanged;

    // Inspector
    g_pInspector_UIControl_chkEnabled = pUIScreen->getChild<onut::UICheckBox>("chkEnabled");
    g_pInspector_UIControl_chkVisible = pUIScreen->getChild<onut::UICheckBox>("chkVisible");
    g_pInspector_UIControl_chkClickThrough = pUIScreen->getChild<onut::UICheckBox>("chkClickThrough");
    g_pInspector_UIControl_txtName = pUIScreen->getChild<onut::UIButton>("txtName");
    g_pInspector_UIControl_txtStyle = pUIScreen->getChild<onut::UIButton>("txtStyle");
    g_pInspector_UIControl_txtX = pUIScreen->getChild<onut::UIButton>("txtX");
    g_pInspector_UIControl_txtY = pUIScreen->getChild<onut::UIButton>("txtY");
    g_pInspector_UIControl_chkXPercent = pUIScreen->getChild<onut::UICheckBox>("chkXPercent");
    g_pInspector_UIControl_chkYPercent = pUIScreen->getChild<onut::UICheckBox>("chkYPercent");
    g_pInspector_UIControl_txtWidth = pUIScreen->getChild<onut::UIButton>("txtWidth");
    g_pInspector_UIControl_txtHeight = pUIScreen->getChild<onut::UIButton>("txtHeight");
    g_pInspector_UIControl_chkWidthPercent = pUIScreen->getChild<onut::UICheckBox>("chkWidthPercent");
    g_pInspector_UIControl_chkHeightPercent = pUIScreen->getChild<onut::UICheckBox>("chkHeightPercent");
    g_pInspector_UIControl_chkWidthRelative = pUIScreen->getChild<onut::UICheckBox>("chkWidthRelative");
    g_pInspector_UIControl_chkHeightRelative = pUIScreen->getChild<onut::UICheckBox>("chkHeightRelative");
    g_pInspector_UIControl_chkEnabled = pUIScreen->getChild<onut::UICheckBox>("chkEnabled");
    g_pInspector_UIControl_chkTOP_LEFT = pUIScreen->getChild<onut::UICheckBox>("chkTOP_LEFT");
    g_pInspector_UIControl_chkTOP = pUIScreen->getChild<onut::UICheckBox>("chkTOP");
    g_pInspector_UIControl_chkTOP_RIGHT = pUIScreen->getChild<onut::UICheckBox>("chkTOP_RIGHT");
    g_pInspector_UIControl_chkLEFT = pUIScreen->getChild<onut::UICheckBox>("chkLEFT");
    g_pInspector_UIControl_chkCENTER = pUIScreen->getChild<onut::UICheckBox>("chkCENTER");
    g_pInspector_UIControl_chkRIGHT = pUIScreen->getChild<onut::UICheckBox>("chkRIGHT");
    g_pInspector_UIControl_chkBOTTOM_LEFT = pUIScreen->getChild<onut::UICheckBox>("chkBOTTOM_LEFT");
    g_pInspector_UIControl_chkBOTTOM = pUIScreen->getChild<onut::UICheckBox>("chkBOTTOM");
    g_pInspector_UIControl_chkBOTTOM_RIGHT = pUIScreen->getChild<onut::UICheckBox>("chkBOTTOM_RIGHT");

    g_pInspector_UIControl_chkTOP_LEFT->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkTOP->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkTOP_RIGHT->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkLEFT->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkCENTER->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkRIGHT->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkBOTTOM_LEFT->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkBOTTOM->onCheckChanged = onAlignChkChanged;
    g_pInspector_UIControl_chkBOTTOM_RIGHT->onCheckChanged = onAlignChkChanged;
}
