/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution. 
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met: 
 *
 * - Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer. 
 *
 * - Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution. 
 *
 * - Neither the name of the <ORGANIZATION> nor the names of its contributors 
 *   may be used to endorse or promote products derived from this software without 
 *   specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE. 
 *
 */  
  
// Rowset.h : Declaration of the CCUBRIDRowset
  
#pragma once
  
#include "resource.h"		// main symbols
#include "command.h"
#include "util.h"
#include "ColumnsRowset.h"	// IColumnsRowset ����
#include "RowsetRow.h"		// RowClass�� �����Ѵ�.
  



/*
 * CRowsetImpl�� ���� ����������
 * IOpenRowset::OpenRowset(CCUBRIDSession) Ȥ�� ICommand::Execute(CCUBRIDCommand)����
 * CreateRowset�� ȣ���ϰ� �� �ȿ��� CCUBRIDRowset::Execute�� �ҷ�����. �׷���,
 * CCUBRIDRowset::Execute������ Storage Type�� �迭�� m_rgRowData�� ��� �����͸� �ִ´�.
 * �׷��� Accessor�� �����ϰ� �����͸� Consumer���� �ѱ�� ���� ���� CRowsetImpl��
 * �� �����͸� ���� �ڵ����� �Ѵ�.
 * Schema Rowset�� ���������� �� ������ �״�� ����Ѵ�.
 *
 * Rowset�� ��쿡�� Schema Rowset�� ���� ������ Storage�� ����
 * ������ �Ѵٰ� �ص� ��� �����͸� �޸𸮿� ���� ���� �� ���� ��쵵 �ֱ� ������
 * �״�� ����� �� ����(Schema Rowset�� ������ ���� ���� ��찡 ���ٰ� �����Ѵ�).
 * �׷��� ������ ���� �����ؼ� �����ߴ�.
 *
 * Stroage�� CDummy�� ������ �ʴ´�.
 * Array of Storage(m_rgRowData)�� �ܼ��� �������� ������ ���ؼ��� �����Ѵ�.
 * ���� �Լ����� ���ȴ�(�������� m_rgRowData�� ������� �ʵ��� �籸��)
 *		IRowsetImpl::GetNextRowsSkipDeleted
 *		IRowsetImpl::GetNextRows
 *		IRowsetLocateImpl::GetRowsAt
 * ���� �����ʹ� GetNextRows�� �����ͼ� RowClass�� �����ϰ�
 * ReleaseRows���� ��������.
 */ 
  
// ���� ��ȯ�ϰ� �ٽ� üũ�ϴ� ���� ���ϱ� ����
// Ŭ������ ����� �ƴ� ��ũ�η� �������.
// TODO: IRowsetImpl::RestartPosition������ row handle�� ���µ�
// �˻��� CHANGED�� ���� ��ȯ�ϴµ� ������� �´� ���� �𸣰ڴ�.
#define CHECK_CANHOLDROWS(iid)														\
  do
  \
  {
    \
    \
    \
      \
  \
while (0)

#define CHECK_RESTART(iid)															\
  do
  \
  {				/* ���� �߻� or ���� ����� */
    \
      /* if(m_nStatus==2) Reexecute(); */ \
      /* if(m_nStatus==2) return RaiseError(E_FAIL, 1, iid, "You must call IRowset::RestartPosition before this"); */ \
  }
while (0)

/*
 * 1. IRowsetChange�� CANHOLDROWS�� exclusive �ϰ�
 *
 * 2. OWNUPDATEDELETE�� OTHERUPDATEDELETE�� ���� ���� ������
 *
 *		CCI ������ ��¿ �� ����. Consumer�� �� �� set �� ���
 *		���߿� set �Ǵ� ���� �ȴ�. CONFLICT�� ���� �Ϸ���
 *		�⺻������ Consumer�� ���� set �� ������ ������ �� �־�� �ϴµ�
 *		�����ϰԴ� �� �� �� �ϴ�.
 *
 *		1�� �⺻���� FALSE�̹Ƿ� TRUE�� ����ڰ� set �� �ɷ� �ؼ� CONFLICT��
 *		�� �� �ִ�.
 *
 * 3. BOOKMARKS�� TRUE�� CANSCROLLBACKWARDS, CANFETCHBACKWARDS�� TRUE��
 *
 * 4. IRowsetLocate�� TRUE�� IRowsetScroll�� TRUE��
 *
 *		�ӽ�. ADO���� IRowsetScroll�� �Լ��� ȣ���ϸ鼭
 *		DBPROP_IRowsetScroll�� ��û���� �ʴ´�.
 *
 * 5. DBPROP_UPDATABILITY, DBPROP_BOOKMARKS�� DBPROP_IRowsetChange, DBPROP_IRowsetLocate��
 *	 CONFLICT���� �ʵ���
 *
 * 6. IRowsetFind�� TRUE�� BOOKMARKS�� TRUE��
 *
 *		LTM�� ���� IRowsetFind �Ӽ��� ������ ��û�ϴ� ���� ���� �� ����. �ϴ� �ּ�ó��.
 */ 
  template < class T > 
{
  
  
    
    {
      
	// case DBPROP_IRowsetChange:
	// case DBPROP_CANHOLDROWS:
	// {
	// CComVariant var;
	// pT->GetPropValue(&DBPROPSET_ROWSET, (pDBProp->dwPropertyID==DBPROP_CANHOLDROWS?
	// DBPROP_IRowsetChange:DBPROP_CANHOLDROWS), &var);
	// if(V_BOOL(&pDBProp->vValue)==ATL_VARIANT_TRUE && V_BOOL(&var)==ATL_VARIANT_TRUE)
	// {
	// pDBProp->dwStatus = DBPROPSTATUS_CONFLICTING;
	// var = false;
	// pT->SetPropValue(&DBPROPSET_ROWSET, pDBProp->dwPropertyID, &var);
	// return E_FAIL;
	// }
	// }
	// break;
    
    
      
      {
	
			   (pDBProp->dwPropertyID ==
			    DBPROP_OWNUPDATEDELETE ? 
			   &pDBProp->vValue);
      
      
    
      
	
	{			// IRowsetChange�� default FALSE����, set FALSE���� �����ؼ� TRUE ����, ������
	  // �����Ѵ�.
	  UPROPVAL *pUPropVal =
	    &(pT->m_pUProp[iCurSet].pUPropVal[pT->GetUPropValIndex (iCurSet, DBPROP_IRowsetChange)]);
	  
	    
	    {
	      
		
		{
		  
		  
		  
		  
		
	      
	      else
		
		{
		  
		  
		
	    
	
      
    
      
	
	{
	  
	  
	
      
    
    
      
	
	{
	  
	  
	  
	  
	  
	    
	    {
	      
	      
	      
	      
	      
	      
	    
	
      
    
      
	
	{
	  
	    // DBPROP_CANSCROLLBACKWARDS�� default FALSE����, set FALSE���� �����ؼ� TRUE ����, ����� �����Ѵ�.
	    UPROPVAL * pUPropVal =
	    &(pT->m_pUProp[iCurSet].pUPropVal[pT->GetUPropValIndex (iCurSet, DBPROP_CANSCROLLBACKWARDS)]);
	  
	    
	    {
	      
		
	    
	  
	    // DBPROP_CANFETCHBACKWARDS�� default FALSE����, set FALSE���� �����ؼ� TRUE ����, ����� �����Ѵ�.
	    pUPropVal = &(pT->m_pUProp[iCurSet].pUPropVal[pT->GetUPropValIndex (iCurSet, DBPROP_CANFETCHBACKWARDS)]);
	  
	    
	    {
	      
		
	    
	  
	
      
      
      
	// case DBPROP_IRowsetFind:
	// if(V_BOOL(&pDBProp->vValue)==ATL_VARIANT_TRUE)
	// {
	// pT->SetPropValue(&DBPROPSET_ROWSET, DBPROP_BOOKMARKS, &pDBProp->vValue);
	// pT->SetPropValue(&DBPROPSET_ROWSET, DBPROP_CANFETCHBACKWARDS, &pDBProp->vValue);
	// pT->SetPropValue(&DBPROPSET_ROWSET, DBPROP_CANSCROLLBACKWARDS, &pDBProp->vValue);
	// }
	// break;
    }
  

{


  
  CCUBRIDCommand >, 
  // IRowsetChange�� �����Ѵ�.
  public ISupportErrorInfo, 
{
  
    // ===== Rowset Ÿ��
public:
  { Invalid, FromSession, FromCommand, FromRow } m_eType;
  
    // ===== Helper
public:
  
  
  
  
  
    // ===== �ڵ�
private:
public:
  
  
    // ===== �������� �ǰ� �ϱ� ���� ����, �Լ�. �ƹ� �ϵ� ���� �ʴ´�.
public:
  HRESULT Execute (DBPARAMS * pParams, DBROWCOUNT * pcRowsAffected) 
  {
    
  
  
    // ===== Initialize, Finalize
private:

  
  
    // TODO_REMOVE: hReq
    HRESULT ValidateCommandID (DBID * pTableID, DBID * pIndexID);	// called by IOpenRowset::OpenRowset
  HRESULT InitFromSession (DBID * pTID, char flag);	// called by IOpenRowset::OpenRowset
  HRESULT InitFromCommand (int hReq, int cResult, bool bAsynch = false);	// called by ICommand::Execute
//      HRESULT InitFromRow(int hReq, int cResult); // called by IRow::Open
  
    // ===== ��Ÿ ���
private:
  

  CComBSTR m_strTableName;
  
  
    // ===== IUnknown
    // �Ӽ��� FALSE�� �������̽��� ���� QI�� �����ϰ� �Ѵ�.
  static HRESULT WINAPI InternalQueryInterface (void *pThis, 
						void **ppvObject) 
  {
    
    {
      
       
     
    {
      
      {
      &__uuidof (IRowsetChange), DBPROP_IRowsetChange}, 
      {
      &__uuidof (IRowsetUpdate), DBPROP_IRowsetUpdate}, 
      {
      &__uuidof (IRowsetLocate), DBPROP_IRowsetLocate}, 
      {
    &__uuidof (IRowsetScroll), DBPROP_IRowsetScroll}, 
    
      
      {
	
	  
	  {
	    
	    
	    
	      return E_NOINTERFACE;
	  
      
    
  
  
    // ===== ISupportErrorInfo
    STDMETHOD (InterfaceSupportsErrorInfo) (REFIID riid) 
  {
    
	 || 
	 || 
	 || 
	 || 
	 || 
	 || 
      
    
    else
      
  
  
    // ===== IAccessor
    STDMETHOD (AddRefAccessor) (HACCESSOR hAccessor, DBREFCOUNT * pcRefCount);
  
			       DBLENGTH cbRowSize, 
  
			    DBBINDING ** prgBindings);
  
  
    // ===== IColumnsInfoImpl
private:

  
  
  
  
    // ===== IConvertType
    STDMETHOD (CanConvert) (DBTYPE wFromType, DBTYPE wToType, DBCONVERTFLAGS dwConvertFlags);
  
    // ===== IRowset
private:
				DBCOUNTITEM * pcRowsObtained, HROW ** prghRows);

  
			   DBROWSTATUS rgRowStatus[]);
  
  
			    DBCOUNTITEM * pcRowsObtained, HROW ** prghRows);
  
			    DBREFCOUNT rgRefCounts[], 
  
  
    // ===== IRowsetChange : defined in RowsetChange.cpp
    STDMETHOD (DeleteRows) (HCHAPTER hReserved, DBCOUNTITEM cRows, 
  
  
  
    // ===== IRowsetUpdate : defined in RowsetChange.cpp
    STDMETHOD (GetOriginalData) (HROW hRow, HACCESSOR hAccessor, void *pData);
  
			       HROW ** prgPendingRows, 
  
			     DBPENDINGSTATUS rgPendingStatus[]);
  
		     HROW ** prgRowsUndone, DBROWSTATUS ** prgRowStatus);
  
		       HROW ** prgRows, DBROWSTATUS ** prgRowStatus);
  
    // ===== IRowsetIdentity
    STDMETHOD (IsSameRow) (HROW hThisRow, HROW hThatRow);
  
    // ===== IRowsetInfo
    virtual HRESULT OnPropertyChanged (ULONG iCurSet, DBPROP * pDBProp);
  
  
			      
  
  
  
    // ===== IGetRow
    STDMETHOD (GetRowFromHROW) (IUnknown * pUnkOuter, HROW hRow, REFIID riid, IUnknown ** ppUnk);
  
  
    // ===== IRowsetFind
private:
    // GetNextRows�� cRows==0�̸� �׳� ������ ������
    // FindNextRow�� ���� ã�Ҵ� �������� ã�ƺ��� �Ѵ�.
    bool m_bFindForward;	// true: forward, false: backward
public:
			    DBBKMARK cbBookmark, const BYTE * pBookmark, 
			    
  
    // ===== IRowsetRefresh
    STDMETHOD (RefreshVisibleData) (HCHAPTER hChapter, DBCOUNTITEM cRows, 
				    DBCOUNTITEM * pcRowsRefreshed, 
				    DBROWSTATUS ** prgRowStatus);
  
  
    // ===== IRowsetLocate
    STDMETHOD (Compare) (HCHAPTER hReserved, DBBKMARK cbBookmark1, const BYTE * pBookmark1, 
			 const BYTE * pBookmark2, DBCOMPARE * pComparison);
  
			  DBROWOFFSET lRowsOffset, 
  
				  const BYTE * rgpBookmarks[], 
  
		     const BYTE * rgpBookmarks[], 
  
    // ===== IRowsetScroll
    STDMETHOD (GetApproximatePosition) (HCHAPTER hReserved, DBBKMARK cbBookmark, 
					DBCOUNTITEM * pulPosition, DBCOUNTITEM * pcRows);
  
			       DBCOUNTITEM ulDenominator, 
			       HROW ** prghRows);

    COM_INTERFACE_ENTRY (IRowsetLocate) 
    COM_INTERFACE_ENTRY (IRowsetUpdate) 
    COM_INTERFACE_ENTRY (ISupportErrorInfo) 
