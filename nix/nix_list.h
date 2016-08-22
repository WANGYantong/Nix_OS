#ifndef NIX_LIST_H
#define NIX_LIST_H

extern void NIX_ListInit(NIX_LIST * pstrList);
extern void NIX_ListNodeAdd(NIX_LIST * pstrList, NIX_LIST * pstrNode);
extern NIX_LIST *NIX_ListNodeDelete(NIX_LIST * pstrList);
extern void NIX_ListNodeInsert(NIX_LIST * pstrList, NIX_LIST * pstrNode,
			       NIX_LIST * pstrNewNode);
extern NIX_LIST *NIX_ListCurNodeDelete(NIX_LIST * pstrList,
				       NIX_LIST * pstrNode);
extern NIX_LIST *NIX_ListEmpInq(NIX_LIST * pstrList);
extern NIX_LIST *NIX_ListNextNodeEmpInq(NIX_LIST * pstrList,
					NIX_LIST * pstrNode);

#endif
