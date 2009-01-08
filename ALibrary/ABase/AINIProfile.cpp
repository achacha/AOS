/*
Written by Alex Chachanashvili

$Id$
*/
#include "pchABase.hpp"
#include "templateAutoPtr.hpp"
#include "AINIProfile.hpp"
#include "AException.hpp"
#include "AFile_Physical.hpp"
#include "AFilename.hpp"
#include "AFileSystem.hpp"

//a_Leaf of the node that builds the tree
struct _INILeaf
{
  _INILeaf() :
    mp_Next(NULL),
    mnv_Pair(ANameValuePair::REGINI)
  {
  }

  ~_INILeaf()
  {
    delete mp_Next;
  }

  ANameValuePair mnv_Pair;
  _INILeaf*       mp_Next;
};

//a_Node used for building the tree
struct _ININode      
{
  _ININode() :
    mp_Child(NULL),
    mp_Peer(NULL),
    mp_Leaf(NULL),
    mp_Parent(NULL)
  {
  }

  _ININode(const AString& name) :
    mp_Child(NULL),
    mp_Peer(NULL),
    mp_Leaf(NULL),
    mp_Parent(NULL),
    mstr_Name(name)
  {
  }

  //a_Auto-cleaning of children and peers
  //a_Set them to NULL to prevent the cascaded deletes
  ~_ININode()
  {
    delete mp_Leaf;

    _ININode* pNode = mp_Child;
    _ININode* pKill = NULL;
    while (pNode)
    {
      pKill = pNode;
      pNode = pNode->mp_Peer;
      delete pKill;
    }
  }

  void getFullName(AString& str)
  {
    if (mp_Parent)
    {
      mp_Parent->getFullName(str);
      if (!str.isEmpty())
        str.append('/');
      str.append(mstr_Name);
    }
    else
      str.assign(mstr_Name);

  }

  //a_Internals
  AString    mstr_Name;
  _ININode* mp_Child;
  _ININode* mp_Parent;
  _ININode* mp_Peer;
  _INILeaf* mp_Leaf;
};

void AINIProfile::debugDump(std::ostream& os, int indent) const
{
  //a_Header
  ADebugDumpable::indent(os, indent) << "(" << typeid(*this).name() << " @ " << std::hex << this << std::dec << ")" << std::endl;
  
  ADebugDumpable::indent(os, indent+1) << "mstr_Filename=" << mstr_Filename << std::endl;
  ADebugDumpable::indent(os, indent+1) << "mp_Root=" << std::hex << (void *)mp_Root << std::dec << " { " << std::endl;
  _dumpTree(mp_Root, os, indent+2);
  ADebugDumpable::indent(os, indent+1) << "}" << std::endl;
  ADebugDumpable::indent(os, indent) << "}" << std::endl;
}

void AINIProfile::_dumpTree(_ININode* pNode, std::ostream& os, int indent) const
{
  if ( ! pNode)
    return;
  
  ADebugDumpable::indent(os, indent) << "[Start" << (void*)pNode << "]" << std::endl;
  
  _ININode* p = pNode;
  while (p)
  {
    _dumpNode(p, os, indent);
    if (p->mp_Child)
      _dumpTree(p->mp_Child, os, indent+1);

    p = p->mp_Peer;
  }

  ADebugDumpable::indent(os, indent) << "[End" << (void*)pNode << "]" << std::endl;
}

void AINIProfile::_dumpNode(_ININode* pNode, std::ostream& os, int indent) const
{
  if (pNode)
  {
		ADebugDumpable::indent(os, indent) << "name=" << pNode->mstr_Name.c_str() << std::endl;
		ADebugDumpable::indent(os, indent) 
      << "ptr=" << std::hex << (void*)pNode
      << "  child=" << (void*)pNode->mp_Child
      << "  peer=" << (void*)pNode->mp_Peer 
      << "  leaf=" << (void*)pNode->mp_Leaf 
      << "  parent=" << (void*)pNode->mp_Parent << std::endl;

    _INILeaf* pLeaf = pNode->mp_Leaf;
    while (pLeaf)
    {
			AString str;
      pLeaf->mnv_Pair.emit(str);
      ADebugDumpable::indent(os, indent+1) << "leaf={" << str << "}" << std::endl;
      pLeaf = pLeaf->mp_Next;
    }
  }
  else
		ADebugDumpable::indent(os, indent) << "ptr=0x00000000\r\n" << std::endl;
}

void AINIProfile::emit(AOutputBuffer& target) const
{
  _writeINIRecursiveLoop(mp_Root, target);
}

AINIProfile::AINIProfile() :
  mp_Root(NULL)
{
}

AINIProfile::AINIProfile(const AString& filename) :
  mstr_Filename(filename),
  mp_Root(NULL)
{
  parse();
}

AINIProfile::AINIProfile(const AFilename& filename) :
  mp_Root(NULL)
{
  filename.emit(mstr_Filename);
  parse();
}

AINIProfile::~AINIProfile()
{
  if (mp_Root)
  {
    _ININode* pNode = mp_Root->mp_Child;
    _ININode* pKill = NULL;
  
    //a_Iterate immediate children and kill them
    while (pNode)
    {
      pKill = pNode;
      pNode = pNode->mp_Peer;
      delete pKill;
    }

    //a_Remove the last node
    mp_Root->mp_Child = NULL;
    pDelete(mp_Root);
  }
}

void AINIProfile::parse()
{
  const AString s_KeyTrim(" []\t");      //a_Used to trim a key line

  //a_If the file does not exist or not set, do not parse (may be a new file to be created)
  if (mstr_Filename.isEmpty())
    return;

  // If it doesn't exist, assume empty INI
  if (!AFileSystem::exists(AFilename(mstr_Filename, false)))
    return;

  //a_Open file
  AFile_Physical f(mstr_Filename);
  f.open();

  AString str;
  _ININode* pCurrent = NULL;
  bool boolLeaf;
  
  bool boolRun = true;
  if (AConstant::npos == f.readLine(str))
    return;

  do
  {
		//a_Parse non empty
    str.stripLeading();
    if (!str.isEmpty())
    {
      //a_Ignore comment line
      if (str[0] != '#')
      {
        //a_Check if the starting character is that for a key '['
        if (str[0] == '[')
          boolLeaf = false;
        else
          boolLeaf = true;

        if (boolLeaf)
        {
          if (pCurrent)
          {
            //a_Assume no keyless strays are added
            _addLeaf(pCurrent, str);
          }
        }
        else
        {
          //a_Remove junk
          str.stripEntire(s_KeyTrim);

          //a_We now have a key in form of: key0/key1/key2/key3
          pCurrent = _makeKeyNode(str);
        }
      }
      str.clear();
    }
    if (AConstant::npos == f.readLine(str))
    {
      //a_Maybe last entry does not have a return
      if (f.readUntilEOF(str) == 0)
        boolRun = false;
    }
  }
	while (boolRun);
}

_ININode* AINIProfile::_findKeyNode(const AString& str) const
{
  //a_No tree, no find...
  if (!mp_Root)
    return NULL;

  AString key;
  size_t uStart = 0x0;
  _ININode* pCurrent = mp_Root->mp_Child;
  _ININode* pParent = mp_Root;
  while ( (uStart = _getNextKey(str, key, uStart)) != AConstant::npos )
  {
    if (uStart == 0x0)
    {
      //a_Only root specified
      return mp_Root;
    }

    if (key.isEmpty())
    {
      //a_Ignore empty keys (_getNextKey increments uStart)
      continue;
    }

    if ( !pCurrent )
    {
      //a_No current node, add one to the parent node (or root if just started)
      if ( ! pParent)
        return NULL;
      else
      {
        if (pParent->mp_Child)
        {
          //a_Child exists, check if one of the peers is this key
          pCurrent = _findPeer(pParent->mp_Child, key);
					if ( ! pCurrent )
            return NULL;
        }
        else
          return NULL;
      }
    }
    else
    {
      //a_Now find if a peer of this name exists
      pCurrent = _findPeer(pCurrent, key);
			if ( ! pCurrent )
        return NULL;
    }

    pParent = pCurrent;
    pCurrent = pCurrent->mp_Child;
  }

  return pParent;
}

_ININode* AINIProfile::_makeKeyNode(const AString& str)
{
  AString key;
  size_t uStart = 0x0;
  _ININode* pCurrent = NULL;
  _ININode* pParent = mp_Root;
  while ( (uStart = _getNextKey(str, key, uStart)) != AConstant::npos )
  {
    //a_Selecting root node
    if (! uStart && mp_Root)
      return mp_Root;

    if ( !pCurrent )
    {
      //a_No current node, add one to the parent node (or root if just started)
      if ( ! pParent)
      {
        //a_Add the parent and child
        mp_Root = new _ININode;
        
        if (key.isEmpty())
        {
          //a_Only root being added
          pCurrent = mp_Root;
        }
        else
        {
          //a_New key is added as child
          mp_Root->mp_Child = new _ININode(key);
          pCurrent = mp_Root->mp_Child;
          pCurrent->mp_Parent = mp_Root;
        }

        //a_Selecting root node
        if (!uStart)
          return mp_Root;
      }
      else
      {
        if (pParent->mp_Child)
        {
          //a_Child exists, check if one of the peers is this key
          pCurrent = _findPeer(pParent->mp_Child, key);
          if ( !pCurrent )
          {
            pCurrent = _addPeer(pParent->mp_Child, key);
            pCurrent->mp_Parent = pParent;
          }
        }
        else
        {
          //a_No such child create
          AASSERT(this, !pCurrent);
          pCurrent = new _ININode(key);
          pParent->mp_Child = pCurrent;
          pCurrent->mp_Parent = pParent;
        }
      }
    }
    else
    {
      //a_Now find if a peer of this name exists
      _ININode* pAddPeerTo = pCurrent;
      pCurrent = _findPeer(pCurrent, key);
			if ( ! pCurrent )
      {
        pCurrent = _addPeer(pAddPeerTo, key);
        pCurrent->mp_Parent = pAddPeerTo->mp_Parent;
      }
    }

    pParent = pCurrent;
    pCurrent = pCurrent->mp_Child;
  }
  
  return pParent;
}

size_t AINIProfile::_getNextKey(const AString& strIn, AString& key, size_t uStart) const
{
  //a_Pre-clean the leading crap
  AString str = strIn;
  str.stripLeading("/\\ \t\r\n");
  
  if (str.isEmpty())
  {
    //a_Probably requesting root
    return 0x0;
  }

  if (uStart >= str.getSize())
  {
    //a_Assume root node specified
    key.clear();
    return AConstant::npos;
  }
  
  size_t uFind = str.findOneOf("/\\", uStart);
  if (uFind == AConstant::npos)
  {
    key.clear();
    str.peek(key, uStart);
    return str.getSize();
  }
  else
  {
    key.clear();
    str.peek(key, uStart, uFind - uStart);
    return uFind + 0x1;
  }
}

_ININode* AINIProfile::_addPeer(_ININode* pStart, const AString& name)
{
  AASSERT(this, pStart);
  while (pStart->mp_Peer)
  {
    //a_Next peer
    pStart = pStart->mp_Peer;
  }

  pStart->mp_Peer = new _ININode(name);

  return pStart->mp_Peer;
}

_ININode* AINIProfile::_findPeer(_ININode* pStart, const AString& name) const
{
  AASSERT(this, pStart);
  while (pStart)
  {
    //a_Found it
    if (pStart->mstr_Name.compareNoCase(name) == 0x0)
      return pStart;
    
    //a_Next peer
    pStart = pStart->mp_Peer;
  }

  return NULL;
}

_INILeaf* AINIProfile::_addLeaf(_ININode* pNode, const AString& str, bool boolOverwrite)
{
  AASSERT(this, pNode);
  _INILeaf* pLeaf = pNode->mp_Leaf;

  AAutoPtr<_INILeaf> pNew(new _INILeaf, true);
  size_t pos = 0x0;
  pNew->mnv_Pair.parse(str, pos);

  if (pNew->mnv_Pair.getName().isEmpty())
  {
    //a_No name, no addition
    return NULL;
  }

  if ( ! pLeaf )
  {
    //a_No leafs yet
    pNode->mp_Leaf = pNew;
    pNew.setOwnership(false);
    return pNew;
  }
  else  
  {
    _INILeaf* pLast;
    
    do
    {
      if (pLeaf->mnv_Pair.isNameNoCase(pNew->mnv_Pair.getName()))
      {
        //a_Already exists
        if (boolOverwrite)
        {
          //a_Overwrite value
          pLeaf->mnv_Pair = pNew->mnv_Pair;
        }

        //a_Discard duplicate and return the found leaf
        return pLeaf;
      }
      
      pLast = pLeaf;
      pLeaf = pLeaf->mp_Next;
    }
    while ( NULL != pLeaf );

    pLast->mp_Next = pNew;
    pNew.setOwnership(false);
  }

  return pLeaf;
}

void AINIProfile::setFilename(const AString& str)
{ 
  mstr_Filename = str;
}

void AINIProfile::setFilename(const AFilename& filename)
{ 
  mstr_Filename.clear();
  filename.emit(mstr_Filename);
}

const AString& AINIProfile::getFilename() const
{ 
  return mstr_Filename;
}

void AINIProfile::writeNow()
{
  _writeINI(NULL);
}

void AINIProfile::toAFile(AFile& file) const
{
	_writeINIRecursiveLoop(mp_Root, file);
}

void AINIProfile::_writeINI(_ININode* pNode) const
{
  if (!pNode)
    pNode = mp_Root;

  if ( mstr_Filename.isEmpty() )
    ATHROW(this, AException::EmptyFilename);

	if (!pNode)
	{
		//a_Tree is empty, empty the file
		return;
	}
	else
	{
    //a_Recursively iterates through the tree and writes
    AFile_Physical f(mstr_Filename, "wb");
    f.open();
		_writeINIRecursiveLoop(pNode, f);
  }
}

void AINIProfile::_writeINIRecursiveLoop(_ININode* pNode, AOutputBuffer& target) const
{
  if ( ! pNode )
    pNode = mp_Root;

  if ( ! pNode)
    return;

  AString strFullName;
  while (pNode)
  {
    //a_Write node name and content
    if (pNode->mp_Leaf)
    {
      //a_Write self and leafs
      pNode->getFullName(strFullName);
      target.append('[');
      target.append(strFullName);
      target.append(']');
      target.append(AConstant::ASTRING_EOL);

      //a_Now the leafs
      _INILeaf* pLeaf = pNode->mp_Leaf;
      while (pLeaf)
      {
        //a_Write each leaf
        pLeaf->mnv_Pair.emit(target);
        target.append(AConstant::ASTRING_EOL);

        pLeaf = pLeaf->mp_Next;
      }
    }
    if (pNode->mp_Child)
      _writeINIRecursiveLoop(pNode->mp_Child, target);

    pNode = pNode->mp_Peer;
  }
}

//a_Clear all
void AINIProfile::clear()
{
  pDelete(mp_Root);
}

AINIProfile::Iterator AINIProfile::getIterator(const AString& key) const
{
  if (key.isEmpty())
    return Iterator( mp_Root );
  else
    return Iterator( _findKeyNode(key) );
}

AINIProfile::WindingIterator AINIProfile::getWindingIterator(const AString& key) const
{
  if (key.isEmpty())
    return WindingIterator( mp_Root );
  else
    return WindingIterator( _findKeyNode(key) );
}

AINIProfile::UnwindingIterator AINIProfile::getUnwindingIterator(const AString& key) const
{
  if (key.isEmpty())
    return UnwindingIterator( mp_Root );
  else
    return UnwindingIterator( _findKeyNode(key) );
}

//a_Existance check of values
bool AINIProfile::getValue(const AString& key, const AString& name, AOutputBuffer& value) const
{
  _ININode* pNode = _findKeyNode(key);

  if (pNode)
  {
    _INILeaf* pLeaf = pNode->mp_Leaf;
    while (pLeaf)
    {
      if (pLeaf->mnv_Pair.isNameNoCase(name))
      {
        value.append(pLeaf->mnv_Pair.getValue());
        return true;
      }
      
      pLeaf = pLeaf->mp_Next;
    }
  }

  return false;
}

bool AINIProfile::isValue(const AString& key, const AString& name, const AString& value) const
{
  AString str;
  if ( getValue(key, name, str) )
    if (str == value)
      return true;

  //a_Either not found or not equal
  return false;
}

//a_Check existance only
bool AINIProfile::exists(const AString& key, const AString& name) const
{
  _ININode* pNode = _findKeyNode(key);

  if (pNode)
  {
    _INILeaf* pLeaf = pNode->mp_Leaf;
    while (pLeaf)
    {
      if (pLeaf->mnv_Pair.isNameNoCase(name))
        return true;
      
      pLeaf = pLeaf->mp_Next;
    }
  }

  return false;
}

void AINIProfile::setValue(const AString& key, const AString& name, const AString& value, bool boolWrite)
{
  _ININode* pNode = NULL;
  if (key.isEmpty())
    pNode = _makeKeyNode("/");
  else
    pNode = _makeKeyNode(key);
  AASSERT(this, pNode);

  AString strPair = name + "=" + value;
  _addLeaf(pNode, strPair, true);             //a_Add leaf and overwrite if needed

  //a_Save if needed
  if (boolWrite)
  {
    _writeINI(NULL);
  }
}

void AINIProfile::removeValue(const AString& key, const AString& name, bool boolWrite)
{
  _ININode* pNode = _findKeyNode(key);

  if (pNode)
  {
    _INILeaf* pLeaf = pNode->mp_Leaf;
    _INILeaf* pLeafLast = NULL;
    while (pLeaf)
    {
      if (pLeaf->mnv_Pair.isNameNoCase(name))
      {
        if (pLeafLast)
        {
          //a_Relink then remove
          pLeafLast->mp_Next = pLeaf->mp_Next;
        }
        else
        {
          //a_No previous (first one)
          pNode->mp_Leaf = pLeaf->mp_Next;
        }

        pLeaf->mp_Next = NULL;    //a_Prevent a chain delete (see dtor for _INILeaf)
        delete pLeaf;
        break;
      }
      pLeafLast = pLeaf;
      pLeaf = pLeaf->mp_Next;
    }
  }

  //a_Save if needed
  if (boolWrite)
    _writeINI(NULL);
}

void AINIProfile::clearKey(const AString& key, bool boolWrite)
{
  //a_Find the node and remove all leafs, on save empty end-nodes will be lost
  _ININode* pNode = _findKeyNode(key);

  //a_If exists
  if (pNode)
  {
    //a_Set the murder row for extermination and disavow of their knowledge
    _INILeaf* pJWG = pNode->mp_Leaf;
    if (pJWG)
    {
      pNode->mp_Leaf = NULL;
    }

    //a_Let the killings begin
    delete pJWG;

    //a_Save if needed
    if (boolWrite)
      _writeINI(NULL);
  }
}

void AINIProfile::removeKey(const AString& key, bool boolWrite)
{
  //a_Find the node and remove all leafs, on save empty end-nodes will be lost
  _ININode* pNode = _findKeyNode(key);
  
  //a_We have such a node and is the parent
  if (pNode)
  {
    //a_Find this node's peer that points to it
    _ININode *p = pNode->mp_Parent;
    if (!p) p = mp_Root;
    while (p && p->mp_Peer != pNode)
      p = p->mp_Peer;

    //a_Remove the node from the peer chain
    AASSERT(this, p);
    if (p)
			p->mp_Peer = pNode->mp_Peer;

    //a_Let the killing begin
    delete pNode;

    //a_Save if needed
    if (boolWrite)
      _writeINI(NULL);
  }

//  debugDump();
}

//////////////////////////////////////////////////////////////////////
//
// Iterator for AINIProfile
//
void AINIProfile::IteratorBase::getBaseKey(AString& str) const
{
  str.clear();
  if (mpnode_Base)
    mpnode_Base->getFullName(str);
}

void AINIProfile::IteratorBase::getCurrentKey(AString& str) const
{
  str.clear();
  if (mpnode_Current)
    mpnode_Current->getFullName(str);
}

void AINIProfile::IteratorBase::getRelativeKey(AString& str) const
{
  AString strThis;
  if (mpnode_Current)
    mpnode_Current->getFullName(strThis);

  AString strBase;
  if (mpnode_Base)
    mpnode_Base->getFullName(strBase);

  str.clear();
  strThis.peek(str, strBase.getSize() + 0x1); 
}

void AINIProfile::IteratorBase::getCurrentKeyName(AString& str) const
{
  str.clear();
  if (mpnode_Current)
    str = mpnode_Current->mstr_Name;
}

void AINIProfile::IteratorBase::getName(AString& str) const
{
  str.clear();
  if (mpleaf_Current)
    str.assign(mpleaf_Current->mnv_Pair.getName());
}

bool AINIProfile::IteratorBase::isNameEqualTo(const AString& str) const
{
  if (mpleaf_Current)
    return mpleaf_Current->mnv_Pair.isNameNoCase(str);
  else
    return false;
}

void AINIProfile::IteratorBase::getValue(AString& str) const
{
  str.clear();
  if (mpleaf_Current)
    str.assign(mpleaf_Current->mnv_Pair.getValue());
}

void AINIProfile::IteratorBase::emit(AOutputBuffer& target) const
{
  if (mpleaf_Current)
    mpleaf_Current->mnv_Pair.emit(target);
}

bool AINIProfile::IteratorBase::isLast() const
{
  return (mpnode_Current ? false : true);
}

void AINIProfile::IteratorBase::clear()
{
  mpnode_Current = mpnode_Base;
  mpleaf_Current = NULL;
}

void AINIProfile::IteratorBase::_copy(const IteratorBase& i)
{
  mpnode_Base    = i.mpnode_Base;
  mpnode_Current = i.mpnode_Current;
  mpleaf_Current = i.mpleaf_Current;
}

bool AINIProfile::IteratorBase::find(const AString& name)
{
  do
  {
    if ( ! next() )
      return false;
  } while ( mpleaf_Current && ! mpleaf_Current->mnv_Pair.isNameNoCase(name) );

  return true;
}

bool AINIProfile::IteratorBase::nextKeyWithLeaf()
{  
  //a_Check if we are at the end or we have an empty iterator
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Force a find of the next key node with a leaf
  mpleaf_Current = NULL;

  while ( ! mpleaf_Current )
  {
    if (mpnode_Current->mp_Child)
    {
      //a_Go in one level
      mpnode_Current = mpnode_Current->mp_Child;
      mpleaf_Current = mpnode_Current->mp_Leaf;
    }
    else
    {
      //a_Go to next peer
      _ININode* pNext = mpnode_Current->mp_Peer;
      if ( ! pNext )
      {
        //a_Last peer of this parent go up one level
        pNext = mpnode_Current->mp_Parent;
        
        //a_Check if a peer exists if not traverse up the parent tree until a peer is found
        if ( pNext->mp_Peer )
          pNext = pNext->mp_Peer;
        else while ( ! pNext->mp_Peer )
        {
          //a_Current node is the base node, thus EOL
          if ( pNext == mpnode_Base)
          {
            mpnode_Current = NULL;    //a_End of list
            return false;
          }
          
          //a_Keep going up until a peer is encountered or a parent key is found
          pNext = pNext->mp_Parent;
          
          //a_The parent node is the base node thus EOL
          if ( pNext == mpnode_Base)
          {
            mpnode_Current = NULL;    //a_End of list
            return false;
          }

          if ( pNext->mp_Peer )
          {
            pNext = pNext->mp_Peer;
            break;
          }
        }
      }

      mpnode_Current = pNext;
      mpleaf_Current = mpnode_Current->mp_Leaf;
    }
  }

  return true;
}

bool AINIProfile::IteratorBase::nextKeyChild()
{  
  //a_Check if we are at the end or we have an empty iterator
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Force a find of the next key node with a leaf
  mpnode_Current = mpnode_Current->mp_Child;

  //a_We have reached the end
  if ( ! mpnode_Current )
    return false;
  else
    return true;
}

bool AINIProfile::IteratorBase::nextKeyPeer()
{  
  //a_Check if we are at the end or we have an empty iterator
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Force a find of the next key node with a leaf
  mpnode_Current = mpnode_Current->mp_Peer;

  //a_We have reached the end
  if ( ! mpnode_Current )
    return false;
  else
    return true;
}

bool AINIProfile::Iterator::next()
{  
  //a_Check if we are at the end
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Leaf is NULL when first executed
  if ( ! mpleaf_Current )
  {
    if (mpnode_Base->mp_Leaf)
    {
      mpleaf_Current = mpnode_Base->mp_Leaf;
      return true;
    }
  }
  else
    mpleaf_Current = mpleaf_Current->mp_Next;
  
  if (mpleaf_Current)
    return true;
  else
  {
    mpnode_Current = NULL;  //a_This is the end... my only friend the end...
    return false;
  }
}

bool AINIProfile::WindingIterator::next()
{
  //a_Check if we are at the end
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Find the next key node with a leaf
  if (mpleaf_Current)
    mpleaf_Current = mpleaf_Current->mp_Next;
  else
    mpleaf_Current = mpnode_Current->mp_Leaf;

  while ( ! mpleaf_Current )
  {
    if (mpnode_Current->mp_Child)
    {
      //a_Go in one level
      mpnode_Current = mpnode_Current->mp_Child;
      mpleaf_Current = mpnode_Current->mp_Leaf;
    }
    else
    {
      //a_Stop from recursing
      if (mpnode_Current == mpnode_Base)
      {
        mpnode_Current = NULL;    //a_End of list
        return false;
      }
      
      //a_Go to next peer
      _ININode* pNext = mpnode_Current->mp_Peer;
      if ( ! pNext )
      {
        //a_Last peer of this parent go up one level
        pNext = mpnode_Current->mp_Parent;
        
        //a_Gone up far enough, stop, EOL detected
        if ( pNext == mpnode_Base)
        {
          mpnode_Current = NULL;    //a_End of list
          return false;
        }

        //a_Check if a peer exists if not traverse up the parent tree until a peer is found
        if ( pNext->mp_Peer )
          pNext = pNext->mp_Peer;
        else while ( ! pNext->mp_Peer )
        {
          //a_Keep going up until a peer is encountered or a parent key is found
          pNext = pNext->mp_Parent;

          //a_Stop from recursing (we at the base)
          if ( ! pNext ||
               pNext == mpnode_Base )
          {
            mpnode_Current = NULL;    //a_End of list
            return false;
          }
          
          if ( pNext->mp_Peer )
          {
            pNext = pNext->mp_Peer;
            break;
          }
        }
      }

      mpnode_Current = pNext;
      mpleaf_Current = mpnode_Current->mp_Leaf;
    }
  }

  return true;
}

bool AINIProfile::UnwindingIterator::next()
{  
  //a_Check if we are at the end
  if ( ! mpnode_Base || ! mpnode_Current )
    return false;

  //a_Leaf is NULL when first executed
  if ( ! mpleaf_Current )
  {
    if (mpnode_Base->mp_Leaf)
    {
      mpleaf_Current = mpnode_Base->mp_Leaf;
      return true;
    }
  }
  else
    mpleaf_Current = mpleaf_Current->mp_Next;
  
  if (mpleaf_Current)
    return true;
  else
  {
    do
    {
      mpnode_Current = mpnode_Current->mp_Parent;
			if (! mpnode_Current )
        return false;  //a_EOL

      mpleaf_Current = mpnode_Current->mp_Leaf;
    }
    while ( ! mpleaf_Current );

    return true;
  }
}

