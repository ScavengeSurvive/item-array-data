#include "item-array-data.inc"

main() {
	new data[10];
	new cell;
	new Item:itemid;
	new size;

	SetItemTypeMaxArrayData(ItemType:0, 0);
	GetItemTypeArrayDataSize(ItemType:0, size);
	SetItemArrayData(itemid, data, 0);
	GetItemArrayData(itemid, data, 0);
	SetItemArrayDataAtCell(itemid, 0, 0);
	GetItemArrayDataAtCell(itemid, cell, 0);
	SetItemArrayDataSize(itemid, 0);
	GetItemArrayDataSize(itemid, size);
	AppendItemDataArray(itemid, data, 0);
	AppendItemArrayCell(itemid, cell);
	SetItemArrayDataLength(itemid, 0);
	RemoveItemArrayData(itemid);
	SetItemNoResetArrayData(itemid, false);
}
