#include "pchABase.hpp"
#include "ATemplateNode.hpp"

ATemplateNode::ATemplateNode(ATemplate& t) :
  m_ParentTemplate(t)
{
}

ATemplateNode::ATemplateNode(const ATemplateNode& that) :
  m_ParentTemplate(that.m_ParentTemplate)
{
}

ATemplateNode::~ATemplateNode()
{
}
