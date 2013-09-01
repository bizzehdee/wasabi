<!-- TextBox Binding -->

<binding id="textbox" 
         h="20" 
         bgstyle="sunken">
 <box margin="1,1,1,1" 
      bgstyle="sunken" 
      highlightcolor="sys-3dface" 
      shadowcolor="sys-3ddarkshadow">
   <box margin="1,1,1,1">
     <edit id="edit" 
           inherit="value,textcolor,bgcolor,disabledbgcolor,multiline,readonly,password,disabled"/>
   </box>
 </box>
  
  <script>

    m_edit = binding.getElementById("edit");

    forwardEvent(m_edit, "onedit", binding);
    forwardEvent(m_edit, "onchar", binding);
    forwardEvent(m_edit, "oneditidle", binding);
    forwardMethod(m_edit, "selectAll", binding);
    forwardMethod(m_edit, "focus", binding);

  </script>
  
</binding>
