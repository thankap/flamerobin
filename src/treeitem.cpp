/*
Copyright (c) 2004, 2005, 2006 The FlameRobin Development Team

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  $Id$

*/

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <algorithm>
#include <vector>

#include "config/Config.h"
#include "metadata/column.h"
#include "metadata/database.h"
#include "treeitem.h"
//-----------------------------------------------------------------------------
using namespace std;
//-----------------------------------------------------------------------------
TreeItem::TreeItem(myTreeCtrl *tree)
    : Observer()
{
    treeM = tree;
}
//-----------------------------------------------------------------------------
MetadataItem *TreeItem::getObservedMetadata()
{
    // tree item observes only one item.
    return (static_cast<MetadataItem*>(getFirstSubject()));
}
//-----------------------------------------------------------------------------
//! returns tree subnode that points to given metadata object
wxTreeItemId TreeItem::findSubNode(MetadataItem *item)
{
    wxTreeItemIdValue cookie;
    wxTreeItemId id = GetId();
    for (wxTreeItemId i = treeM->GetFirstChild(id, cookie); i.IsOk();
        i = treeM->GetNextChild(id, cookie))
    {
        MetadataItem *s = treeM->getMetadataItem(i);
        if (s == item)
            return i;
    }
    return wxTreeItemId();
}
//-----------------------------------------------------------------------------
//! parent nodes are responsible for "insert" / "delete"
//! node is responsible for "update"
void TreeItem::update()
{
    wxTreeItemId id = GetId();
    if (!id.IsOk())
        return;

    MetadataItem *object = getObservedMetadata();
    if (!object)
        return;

    bool hideDisconnected = config().get(wxT("HideDisconnectedDatabases"),
        false);

    // check current item
    wxString itemText(object->getPrintableName());
    if (treeM->GetItemText(id) != itemText)
        treeM->SetItemText(id, itemText);

    NodeType ndt = object->getType();
    bool affectedBySetting = (ndt == ntTable || ndt == ntView
        || ndt == ntProcedure);
    bool showColumns = !affectedBySetting ||
        config().get(wxT("ShowColumnsInTree"), true);

    // check subitems
    vector<MetadataItem*> children;
    vector<MetadataItem*>::iterator itChild;
    if (showColumns && object->getChildren(children))
    {   // check if some tree node has to be added
        wxTreeItemId prevItem;
        for (itChild = children.begin(); itChild != children.end(); ++itChild)
        {
            // skip autogenerated domains
            if ((*itChild)->getType() == ntDomain && (*itChild)->isSystem())
                continue;
            // hide disconnected databases
            if (hideDisconnected)
            {
                Database* db = dynamic_cast<Database*>(*itChild);
                if (db && !db->isConnected())
                    continue;
            }

            itemText = (*itChild)->getPrintableName();
            wxTreeItemId item = findSubNode(*itChild);

            if (!item.IsOk())
            {   // add if needed
                TreeItem *newItem = new TreeItem(treeM);
                (*itChild)->attachObserver(newItem);
                int image = treeM->getItemImage((*itChild)->getType());
                if (object->getType() == ntTable)
                {
                    Column* c = static_cast<Column*>(*itChild);
                    if (c->isPrimaryKey())
                        image = treeM->getItemImage(ntPrimaryKey);
                    else if (!c->getComputedSource().IsEmpty())
                        image = treeM->getItemImage(ntComputed);
                }

                if (prevItem.IsOk())
                    prevItem = treeM->InsertItem(id, prevItem, itemText, image, -1, newItem);
                else    // first
                    prevItem = treeM->PrependItem(id, itemText, image, -1, newItem);
            }
            else
            {   // change text if needed
                if (treeM->GetItemText(item) != itemText)
                    treeM->SetItemText(item, itemText);
                prevItem = item;
            }
        }
    }

    // remove all children at once
    if (!children.size())
    {
        if (treeM->ItemHasChildren(id))
        {
            if (id != treeM->GetRootItem() ||
                (treeM->GetWindowStyle() & wxTR_HIDE_ROOT) == 0)
            {
                treeM->Collapse(id);
            }
            treeM->DeleteChildren(id);
            treeM->SetItemBold(id, false);
        }
        return;
    }

    // remove delete items - one by one
    bool itemsDeleted = false;
    wxTreeItemIdValue cookie;
    wxTreeItemId item = treeM->GetFirstChild(id, cookie);
    while (item.IsOk())
    {
        itChild = find(children.begin(), children.end(),
            treeM->getMetadataItem(item));
        // we may need to hide disconnected databases
        if (hideDisconnected && itChild != children.end())
        {
            Database* db = dynamic_cast<Database*>(*itChild);
            if (db && !db->isConnected())
                itChild = children.end();
        }
        // delete tree node and all children if metadata item not found
        if (itChild == children.end())
        {
            itemsDeleted = true;
            treeM->DeleteChildren(item);
            treeM->Delete(item);
            item = treeM->GetFirstChild(id, cookie);
            continue;
        }
        item = treeM->GetNextChild(id, cookie);
    }
    // force-collapse node if all children deleted
    if (itemsDeleted && 0 == treeM->GetChildrenCount(id, false) &&
       (item != treeM->GetRootItem() ||
        (treeM->GetWindowStyle() & wxTR_HIDE_ROOT) == 0) )
    {
        treeM->Collapse(id);
    }

    treeM->SetItemBold(id, treeM->ItemHasChildren(id));
    if (object->orderedChildren())
        treeM->SortChildren(id);
}
//-----------------------------------------------------------------------------
