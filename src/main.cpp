#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

using namespace geode::prelude;

// We hook EditorUI to inject our quick-access buttons onto the first
// page (tab 0) of the Edit tab panel.
class $modify(QuickEditUI, EditorUI) {

    // ── Helper: find the edit-tab button bar ──────────────────────────────
    // GD lays out the editor UI as:
    //   m_editButtonBar  →  CCMenu inside it  →  rows of CCMenuItemSpriteExtra
    // Each "page" is a separate CCNode child of m_editButtonBar.
    // Page 0 is the first page (default visible page).

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        this->scheduleOnce(schedule_selector(QuickEditUI::addQuickButtons), 0.0f);
        return true;
    }

    void addQuickButtons(float) {
        // m_editButtonBar is the EditButtonBar (a BoomScrollLayer wrapper).
        // Its first child page holds the actual CCMenu with edit buttons.
        auto* editBar = this->m_editButtonBar;
        if (!editBar) {
            log::error("[QuickEditButtons] Could not find m_editButtonBar");
            return;
        }

        // The EditButtonBar contains a BoomListView / CCLayer hierarchy.
        // Walk children to find the CCMenu on page 0.
        CCMenu* editMenu = findFirstEditMenu(editBar);
        if (!editMenu) {
            log::error("[QuickEditButtons] Could not find edit CCMenu on page 0");
            return;
        }

        // ── Build the four buttons ────────────────────────────────────────
        // We use the same sprite frames GD already ships so they look native.
        struct BtnDef {
            const char* sprite;   // sprite frame name
            SEL_MenuHandler sel;  // selector
            const char* tooltip;
        };

        BtnDef defs[] = {
            { "edit_flipXBtn_001.png",    menu_selector(QuickEditUI::onFlipX),     "Flip X"       },
            { "edit_flipYBtn_001.png",    menu_selector(QuickEditUI::onFlipY),     "Flip Y"       },
            { "edit_cwBtn_001.png",       menu_selector(QuickEditUI::onRotateCW),  "Rotate CW"    },
            { "edit_ccwBtn_001.png",      menu_selector(QuickEditUI::onRotateCCW), "Rotate CCW"   },
        };

        // Determine button size from existing buttons in the menu so we
        // match the visual scale exactly.
        float btnSize = 40.0f;
        if (auto* firstBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(
                editMenu->getChildren()->objectAtIndex(0))) {
            btnSize = firstBtn->getContentSize().width;
        }

        // We'll append a small row at the top of the existing menu.
        // Collect current children so we can nudge them down.
        float padding   = 4.0f;
        float rowHeight = btnSize + padding;

        // Shift all existing children downward to make room at the top.
        CCArray* children = editMenu->getChildren();
        for (unsigned int i = 0; i < children->count(); i++) {
            auto* node = static_cast<CCNode*>(children->objectAtIndex(i));
            node->setPositionY(node->getPositionY() - rowHeight);
        }

        // Insert the four new buttons in a horizontal row.
        float totalW   = (btnSize + padding) * 4 - padding;
        float startX   = -totalW / 2.0f + btnSize / 2.0f;
        // Place them near the top of the menu content area.
        float yPos     = editMenu->getContentSize().height / 2.0f
                         - btnSize / 2.0f - padding;

        for (int i = 0; i < 4; i++) {
            auto& def = defs[i];

            // Create sprite – fall back to a plain colour square if the
            // frame is somehow missing (shouldn't happen on a real GD build).
            CCSprite* spr = CCSprite::createWithSpriteFrameName(def.sprite);
            if (!spr) {
                spr = CCSprite::create();
                spr->setContentSize({ btnSize, btnSize });
            }

            auto* btn = CCMenuItemSpriteExtra::create(
                spr, this, def.sel
            );
            btn->setPosition({
                startX + i * (btnSize + padding),
                yPos
            });
            btn->setTag(1000 + i); // unique tag so we can identify them later

            editMenu->addChild(btn);
        }

        // Expand the menu content height to accommodate the new row.
        editMenu->setContentSize({
            editMenu->getContentSize().width,
            editMenu->getContentSize().height + rowHeight
        });

        log::info("[QuickEditButtons] Successfully injected 4 quick-edit buttons.");
    }

    // ── Walk the node tree to find the CCMenu inside the edit button bar ──
    CCMenu* findFirstEditMenu(CCNode* root) {
        // The structure is roughly:
        //   EditButtonBar (CCLayer)
        //     └── BoomScrollLayer / CCLayer  (page container)
        //           └── CCLayer  (page 0)
        //                 └── CCMenu  ← we want this
        // We do a shallow BFS up to depth 5.
        return findMenuRecursive(root, 0, 5);
    }

    CCMenu* findMenuRecursive(CCNode* node, int depth, int maxDepth) {
        if (!node || depth > maxDepth) return nullptr;
        CCArray* children = node->getChildren();
        if (!children) return nullptr;
        for (unsigned int i = 0; i < children->count(); i++) {
            CCNode* child = static_cast<CCNode*>(children->objectAtIndex(i));
            if (auto* menu = typeinfo_cast<CCMenu*>(child)) {
                return menu;
            }
            if (auto* found = findMenuRecursive(child, depth + 1, maxDepth)) {
                return found;
            }
        }
        return nullptr;
    }

    // ── Button callbacks – delegate straight to EditorUI's own methods ────

    void onFlipX(CCObject* sender) {
        // EditorUI::onFlipX exists in GD 2.2 – flip selected objects on X axis
        this->flipObjectsX(this->getSelectedObjects());
    }

    void onFlipY(CCObject* sender) {
        this->flipObjectsY(this->getSelectedObjects());
    }

    void onRotateCW(CCObject* sender) {
        // Rotate 90° clockwise = -90°
        this->rotateObjects(this->getSelectedObjects(), -90.0f, this->getGroupCenter(this->getSelectedObjects(), false));
    }

    void onRotateCCW(CCObject* sender) {
        // Rotate 90° counter-clockwise = +90°
        this->rotateObjects(this->getSelectedObjects(), 90.0f, this->getGroupCenter(this->getSelectedObjects(), false));
    }
};
