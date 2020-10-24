// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_item_array_data
	#undef _inc_item_array_data
#endif
// custom include-guard to ensure we don't duplicate
#if defined _item_array_data_included
	#endinput
#endif
#define _item_array_data_included


#include <a_samp>
#include <logger>
#include <errors>
#include <item>
#include <YSI_Coding\y_hooks>


#if !defined ITM_ARR_MAX_ARRAY_DATA
	#define ITM_ARR_MAX_ARRAY_DATA (256)
#endif


// Functions


// SetItemTypeMaxArrayData sets the maximum size for array data for a specific
// item type. This should be only called once, preferably immediately after
// the item type has been defined. `protect` will mark items of this type as
// "protected" meaning they require explicit actions to modify.
forward SetItemTypeMaxArrayData(ItemType:itemtype, size, bool:protect = false);

// GetItemTypeArrayDataSize returns the array data size for a particular type.
forward GetItemTypeArrayDataSize(ItemType:itemtype, &size);

// SetItemArrayData is for setting the entire item array at once, the item's
// array size will be resized automatically unless it is protected.
forward Error:SetItemArrayData(Item:itemid, const data[], length, call = 1, bool:force = false);

// GetItemArrayData stores the entire array for an item in `data`.
forward Error:GetItemArrayData(Item:itemid, data[], length = sizeof data);

// SetItemArrayDataAtCell sets a single cell of an item's array data. `autosize`
// adjust the array data automatically if `cell` specifies a cell index beyond
// the current item's array size. `call` controls whether or not to call
// `OnItemArrayDataChanged` and `force` is required for protected item types.
forward Error:SetItemArrayDataAtCell(Item:itemid, data, cell, bool:autoSize = false, bool:call = true, bool:force = false);

// GetItemArrayDataAtCell stores into `data` the data in the item's array at
// `cell`.
forward Error:GetItemArrayDataAtCell(Item:itemid, &data, cell);

// SetItemArrayDataSize adjusts the array size for an item. This size must be
// within the item's max size for its type. `force` is required true for
// protected item types.
forward Error:SetItemArrayDataSize(Item:itemid, size, bool:force = false);

// GetItemArrayDataSize stores into `size` the size of the item's array.
forward GetItemArrayDataSize(Item:itemid, &size);

// AppendItemDataArray appends an entire array to an item's array.
forward Error:AppendItemDataArray(Item:itemid, const data[], length);

// AppendItemArrayCell appends a single cell to an item's array.
forward Error:AppendItemArrayCell(Item:itemid, data);

// SetItemArrayDataLength explicitly sets the array size for an item.
forward Error:SetItemArrayDataLength(Item:itemid, length, bool:force = false);

// RemoveItemArrayData clears all data for an item.
forward RemoveItemArrayData(Item:itemid);

// SetItemNoResetArrayData will ensure an item's data is not reset on
// `OnItemCreate`, this is useful if you want to set some array data when an
// item is created but you have no control over when the `OnItemCreate` hook in
// this library is called. If the hook in this library is called after your own
// hook then using this function will ensure that array data is not zeroed
// immediately after it has been set.
forward SetItemNoResetArrayData(Item:itemid, bool:toggle);

// OnItemArrayDataChanged is called whenever a function modifies an item's data.
forward OnItemArrayDataChanged(Item:itemid);


/*==============================================================================

	Setup

==============================================================================*/


static
	itm_arr_ItemTypeArrayMax[MAX_ITEM_TYPE],
	bool:itm_arr_ItemTypeArrayProtect[MAX_ITEM_TYPE],
	itm_arr_Data[MAX_ITEM][ITM_ARR_MAX_ARRAY_DATA],
	itm_arr_Length[MAX_ITEM],
	itm_arr_NoResetOnCreate[MAX_ITEM];


hook OnItemCreate(Item:itemid) {
	if(!itm_arr_NoResetOnCreate[itemid]) {
		for(new i; i < itm_arr_ItemTypeArrayMax[GetItemType(itemid)]; i++) {
			itm_arr_Data[itemid][i] = 0;
		}

		itm_arr_Length[itemid] = 0;
	} else {
		itm_arr_NoResetOnCreate[itemid] = 0;
	}

	return 1;
}

stock SetItemTypeMaxArrayData(ItemType:itemtype, size, bool:protect = false) {
	if(!IsValidItemType(itemtype)) {
		Logger_Err("attempt to set max array data for invalid item type",
			Logger_I("itemtype", _:itemtype));
		return 1;
	}

	if(size > ITM_ARR_MAX_ARRAY_DATA) {
		Logger_Err("attempt to set max array data beyond hard limit",
			Logger_I("size", size),
			Logger_I("max", ITM_ARR_MAX_ARRAY_DATA),
			Logger_I("itemtype", _:itemtype));
		return 2;
	}

	itm_arr_ItemTypeArrayMax[itemtype] = size;
	itm_arr_ItemTypeArrayProtect[itemtype] = protect;

	return 0;
}

stock GetItemTypeArrayDataSize(ItemType:itemtype, &size) {
	if(!IsValidItemType(itemtype)) {
		return 1;
	}

	size = itm_arr_ItemTypeArrayMax[itemtype];
	return 0;
}

stock Error:SetItemArrayData(Item:itemid, const data[], length, call = 1, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(length > itm_arr_ItemTypeArrayMax[itemtype]) {
		return Error(2, "length exceeds item type max data size");
	}

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		return Error(3, "itemtype uses data protection, must force");
	}

	itm_arr_Length[itemid] = 0;

	for(new i; i < length; i++) {
		itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data[i];
	}

	if(call) {
		CallLocalFunction("OnItemArrayDataChanged", "d", _:itemid);
	}

	return NoError();
}

stock Error:GetItemArrayData(Item:itemid, data[], length = sizeof data) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(length > itm_arr_ItemTypeArrayMax[itemtype]) {
		length = itm_arr_ItemTypeArrayMax[itemtype];
	}

	for(new i; i < itm_arr_Length[itemid] && i < length; i++) {
		data[i] = itm_arr_Data[itemid][i];
	}

	return NoError();
}

stock Error:SetItemArrayDataAtCell(Item:itemid, data, cell, bool:autoSize = false, bool:call = true, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(cell >= itm_arr_ItemTypeArrayMax[itemtype]) {
		return Error(2, "attempt to write data beyond item type max");
	}

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		return Error(3, "itemtype uses data protection, must force");
	}

	if(cell >= itm_arr_Length[itemid]) {
		if(autoSize) {
			itm_arr_Length[itemid] = cell + 1;
		} else {
			return Error(4, "attempt to write data beyond item array size");
		}
	}

	itm_arr_Data[itemid][cell] = data;

	if(call) {
		CallLocalFunction("OnItemArrayDataChanged", "d", _:itemid);
	}

	return NoError();
}

stock Error:GetItemArrayDataAtCell(Item:itemid, &data, cell) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(cell >= itm_arr_ItemTypeArrayMax[itemtype]) {
		return Error(2, "attempt to read data beyond item type max");
	}

	if(cell >= itm_arr_Length[itemid]) {
		return Error(3, "attempt to read data beyond data length");
	}

	data = itm_arr_Data[itemid][cell];
	return NoError();
}

stock Error:SetItemArrayDataSize(Item:itemid, size, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		return Error(2, "itemtype uses data protection, must force");
	}

	itm_arr_Length[itemid] = size;

	return NoError();
}

stock GetItemArrayDataSize(Item:itemid, &size) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	size = itm_arr_Length[itemid];
	return 0;
}

stock Error:AppendItemDataArray(Item:itemid, const data[], length) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	if(itm_arr_Length[itemid] + length > itm_arr_ItemTypeArrayMax[GetItemType(itemid)]) {
		return Error(2, "append out of bounds of item type max size");
	}

	for(new i; i < length; i++) {
		itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data[i];
	}

	return NoError();
}

stock Error:AppendItemArrayCell(Item:itemid, data) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	if(itm_arr_Length[itemid] + 1 > itm_arr_ItemTypeArrayMax[GetItemType(itemid)]) {
		return Error(2, "append out of bounds of item type max size");
	}

	itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data;

	return NoError();
}

stock Error:SetItemArrayDataLength(Item:itemid, length, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return Error(1, "invalid item id");
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		return Error(2, "itemtype uses data protection, must force");
	}

	itm_arr_Length[itemid] = length;

	return NoError();
}

stock RemoveItemArrayData(Item:itemid) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	itm_arr_Length[itemid] = 0;
	return 0;
}

stock SetItemNoResetArrayData(Item:itemid, bool:toggle) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	itm_arr_NoResetOnCreate[itemid] = toggle;
	return 0;
}
