#include "item-array-data.inc"

main() {
	new data[10];
	new cell;

	SetItemTypeMaxArrayData(ItemType:0, 0);
	GetItemTypeArrayDataSize(ItemType:0);
	SetItemArrayData(0, data, 0);
	GetItemArrayData(0, data, 0);
	SetItemArrayDataAtCell(0, 0, 0);
	GetItemArrayDataAtCell(0, 0, cell);
	SetItemArrayDataSize(0, 0);
	GetItemArrayDataSize(0);
	GetItemTypeArrayDataMax(ItemType:0);
	AppendItemDataArray(0, data, 0);
	AppendItemArrayCell(0, cell);
	SetItemArrayDataLength(0, 0);
	RemoveItemArrayData(0);
	SetItemNoResetArrayData(0, false);
}
