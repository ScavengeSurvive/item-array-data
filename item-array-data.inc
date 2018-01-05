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
#include <item>
#include <YSI\y_hooks>


#if !defined ITM_ARR_MAX_ARRAY_DATA
	#define ITM_ARR_MAX_ARRAY_DATA (256)
#endif

#if !defined ITM_ARR_ARRAY_SIZE_PROTECT
	#define ITM_ARR_ARRAY_SIZE_PROTECT (true)
#endif


// Functions


forward SetItemTypeMaxArrayData(ItemType:itemtype, size, bool:protect = false);
/*
# Description:
-
*/

forward GetItemTypeArrayDataSize(ItemType:itemtype);
/*
# Description:
-
*/

forward SetItemArrayData(itemid, data[], length, call = 1, bool:force = false);
/*
# Description:
-
*/

forward GetItemArrayData(itemid, data[], length = sizeof data);
/*
# Description:
-
*/

forward SetItemArrayDataAtCell(itemid, data, cell, autoadjustsize = 0, call = 1, bool:force = false);
/*
# Description:
-
*/

forward GetItemArrayDataAtCell(itemid, cell, &data);
/*
# Description:
-
*/

forward SetItemArrayDataSize(itemid, size, bool:force = false);
/*
# Description:
-
*/

forward GetItemArrayDataSize(itemid);
/*
# Description:
-
*/

forward GetItemTypeArrayDataMax(ItemType:itemtype);
/*
# Description:
-
*/

forward AppendItemDataArray(itemid, data[], length);
/*
# Description:
-
*/

forward AppendItemArrayCell(itemid, data);
/*
# Description:
-
*/

forward SetItemArrayDataLength(itemid, length, bool:force = false);
/*
# Description:
-
*/

forward RemoveItemArrayData(itemid);
/*
# Description:
-
*/

forward SetItemNoResetArrayData(itemid, bool:toggle);
/*
# Description:
-
*/


// Events


forward OnItemArrayDataChanged(itemid);
/*
# Called:
-
*/


/*==============================================================================

	Setup

==============================================================================*/


static
	itm_arr_ItemTypeArrayMax[MAX_ITEM_TYPE],
	bool:itm_arr_ItemTypeArrayProtect[MAX_ITEM_TYPE],
	itm_arr_Data[MAX_ITEM][ITM_ARR_MAX_ARRAY_DATA],
	itm_arr_Length[MAX_ITEM],
	itm_arr_NoResetOnCreate[MAX_ITEM];


hook OnItemCreate(itemid) {
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
		return 1;
	}

	if(size > ITM_ARR_MAX_ARRAY_DATA) {
		return 2;
	}

	itm_arr_ItemTypeArrayMax[itemtype] = size;
	itm_arr_ItemTypeArrayProtect[itemtype] = protect;

	return 0;
}

stock GetItemTypeArrayDataSize(ItemType:itemtype) {
	if(!IsValidItemType(itemtype)) {
		return -1;
	}

	return itm_arr_ItemTypeArrayMax[itemtype];
}

stock SetItemArrayData(itemid, data[], length, call = 1, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return 0;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(length > itm_arr_ItemTypeArrayMax[itemtype]) {
		err("data length exceeds item type max data size.",
			_i("itemtype", _:itemtype),
			_i("length", length),
			_i("maxLength", itm_arr_ItemTypeArrayMax[GetItemType(itemid)]));
		return 1;
	}

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		err("itemtype (%d) uses data protection and force parameter is false.",
			_i("itemtype", _:itemtype));
		return 2;
	}

	itm_arr_Length[itemid] = 0;

	for(new i; i < length; i++) {
		itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data[i];
	}

	if(call) {
		CallLocalFunction("OnItemArrayDataChanged", "d", itemid);
	}

	return 0;
}

stock GetItemArrayData(itemid, data[], length = sizeof data) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(length != itm_arr_ItemTypeArrayMax[itemtype]) {
		err("destination length does not match item type max length",
			_i("itemtype", _:itemtype),
			_i("length", length),
			_i("max", itm_arr_ItemTypeArrayMax[itemtype]));
	}

	for(new i; i < itm_arr_Length[itemid]; i++) {
		data[i] = itm_arr_Data[itemid][i];
	}

	return 0;
}

stock SetItemArrayDataAtCell(itemid, data, cell, autoadjustsize = 0, call = 1, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(cell >= itm_arr_ItemTypeArrayMax[itemtype]) {
		err("attempt to write item array data beyond item type max",
			_i("itemtype", _:itemtype),
			_i("cell", cell),
			_i("max", itm_arr_ItemTypeArrayMax[itemtype]));

		return 2;
	}

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		err("itemtype uses data protection and force parameter is false",
			_i("itemtype", _:itemtype));
		return 3;
	}

	if(cell >= itm_arr_Length[itemid]) {
		if(autoadjustsize) {
			itm_arr_Length[itemid] = cell + 1;
		} else {
			#if ITM_ARR_ARRAY_SIZE_PROTECT == false
				itm_arr_Length[itemid] = cell + 1;
			#else
				return 4;
			#endif	
		}
	}

	itm_arr_Data[itemid][cell] = data;

	if(call) {
		CallLocalFunction("OnItemArrayDataChanged", "d", itemid);
	}

	return 0;
}

stock GetItemArrayDataAtCell(itemid, cell, &data) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(cell >= itm_arr_ItemTypeArrayMax[itemtype]) {
		err("attempt to read item array data beyond item type max",
			_i("itemtype", _:itemtype),
			_i("cell", cell),
			_i("max", itm_arr_ItemTypeArrayMax[itemtype]));
		return 2;
	}

	#if ITM_ARR_ARRAY_SIZE_PROTECT == true
	if(cell >= itm_arr_Length[itemid]) {
		err("attempt to read item array data beyond data length",
			_i("itemtype", _:itemtype),
			_i("cell", cell),
			_i("max", itm_arr_ItemTypeArrayMax[itemtype]));
		return 3;
	}
	#endif

	data = itm_arr_Data[itemid][cell];
	return 0;
}

stock SetItemArrayDataSize(itemid, size, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		err("itemtype uses data protection and force parameter is false",
			_i("itemtype", _:itemtype));
		return 2;
	}

	itm_arr_Length[itemid] = size;

	return 0;
}

stock GetItemArrayDataSize(itemid) {
	if(!IsValidItem(itemid)) {
		return 0;
	}

	return itm_arr_Length[itemid];
}

stock GetItemTypeArrayDataMax(ItemType:itemtype) {
	if(!IsValidItemType(itemtype)) {
		return 0;
	}

	return itm_arr_ItemTypeArrayMax[itemtype];
}

stock AppendItemDataArray(itemid, data[], length) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	if(itm_arr_Length[itemid] + length > itm_arr_ItemTypeArrayMax[GetItemType(itemid)]) {
		return 2;
	}

	for(new i; i < length; i++) {
		itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data[i];
	}

	return 0;
}

stock AppendItemArrayCell(itemid, data) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	if(itm_arr_Length[itemid] + 1 > itm_arr_ItemTypeArrayMax[GetItemType(itemid)]) {
		return 2;
	}

	itm_arr_Data[itemid][itm_arr_Length[itemid]++] = data;

	return 0;
}

stock SetItemArrayDataLength(itemid, length, bool:force = false) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	new ItemType:itemtype = GetItemType(itemid);

	if(itm_arr_ItemTypeArrayProtect[itemtype] && !force) {
		err("itemtype (%d) uses data protection and force parameter is false.",
			_i("itemtype", _:itemtype));
		return 2;
	}

	itm_arr_Length[itemid] = length;

	return 0;
}

stock RemoveItemArrayData(itemid) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	itm_arr_Length[itemid] = 0;

	return 0;
}

stock SetItemNoResetArrayData(itemid, bool:toggle) {
	if(!IsValidItem(itemid)) {
		return 1;
	}

	itm_arr_NoResetOnCreate[itemid] = toggle;

	return 0;
}
