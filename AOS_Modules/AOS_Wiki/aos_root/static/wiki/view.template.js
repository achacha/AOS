function clickEdit(me, wiki_id, form_id)
{
  var eEditLink = document.getElementById(me);
  var eWikiBody = document.getElementById(wiki_id);
  var eEditForm = document.getElementById(form_id);
  
  //Hide the existing body
  eWikiBody.style.display = 'none';

  //a_Create text area
  var eTextArea = document.createElement("textarea");
  eTextArea.innerHTML = eWikiBody.innerHTML;
  eTextArea.className = "wikibody_edit";
  eTextArea.setAttribute("cols", "200");
  eTextArea.setAttribute("rows", "30");
  eTextArea.setAttribute("id", "wikibody_edit_textarea");
  eTextArea.setAttribute("name", "newdata");
  eTextArea.style.display = 'none';

  //a_Create submit button
  var eEditSaveButton = document.createElement("input");
  eEditSaveButton.setAttribute("id", "wikibody_edit_submit_button");
  eEditSaveButton.setAttribute("value", "Save");
  eEditSaveButton.setAttribute("type", "submit");
  
  //a_Add debug dumpContext input
  var eDebugInput = document.createElement("input");
  eDebugInput.setAttribute("name", "dumpContext");
  eDebugInput.setAttribute("type", "checkbox");
  eDebugInput.setAttribute("value", "2");
  
  //a_Add to FORM
  eEditForm.appendChild(eTextArea);
  eEditForm.appendChild(eEditSaveButton);
  eEditForm.appendChild(eDebugInput);
  
  //a_Create MCE editor out of the textarea
  tinyMCE.execCommand('mceAddControl', false, eTextArea.id);
  
  eEditLink.innerHTML = "undo edit";
  eEditLink.setAttribute("onClick", "javascript: return clickUndoEdit('"+eEditLink.id+"','"+eWikiBody.id+"','"+eEditForm.id+"');");

  return 0;
}

function clickUndoEdit(me, wiki_id, form_id)
{
  var eEditLink = document.getElementById(me);
  var eWikiBody = document.getElementById(wiki_id);
  var eEditForm = document.getElementById(form_id);
  
  eWikiBody.style.display = '';
  while (eEditForm.hasChildNodes())
  {
    eEditForm.removeChild(eEditForm.lastChild);
  }

  eEditLink.innerHTML = "edit";
  eEditLink.setAttribute("onClick", "javascript: return clickEdit('"+eEditLink.id+"','"+eWikiBody.id+"','"+eEditForm.id+"');");
  
  return 0;
}
