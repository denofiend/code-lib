Ext.require([
  'Ext.window.MessageBox'
]);

Ext.application({
  name: 'job_service',
  launch: function() {
    function datetime_renderer(value){
      var d = new Date(value*1000);
      return ''+d.getFullYear()+'-'+(d.getMonth()+1)+'-'+d.getDate()+' '+d.toLocaleTimeString();
    };

    var WIDTH = 1024;
    var HEIGHT = 600;

	function get_source(){
		var source;
		try{
			source = sourceText.getValue();
		}catch(error){
			return false;
		}
		return source;
	}

    function computersize_renderer(value){
      if(value>1000000000){
        return (value/1000000000).toFixed(2)+'G';
      }else if(value>1000000){
        return (value/1000000).toFixed(2)+'M';
      }else if(value>1000){
        return (value/1000).toFixed(2)+'K';
      }
      return value+'B';
    };

    function sourceid_renderer(value){
      return value.toUpperCase();
    };

    var apps = Ext.create('Ext.data.Store', {
      fields: ['app', 'label'],
      data : [
        {"app":"tbsc", "label":"云标签"},
        {"app":"disk", "label":"云下载"},
        {"app":"dsnd", "label":"云传送"}
      ]
    });

    Ext.define('MxDevice',{
      extend: 'Ext.data.Model',
      fields: [
        'id',
        'status',
        'title',
        'description',
        'pushdate',
        'url',
        'ifimg',
        'lang',
        'creator',
        'updatetime']
    });
    Ext.define('MxFile',{
      extend: 'Ext.data.Model',
      fields:[
        'path',
        'mt',
        'size']
    });
    Ext.define('MxMessage',{
      extend: 'Ext.data.Model',
      fields:[
        'from_source',
        'to_source',
        'screen_name',
        'type',
        'path']
    });

    Ext.define('MxSourceId',{
      extend: 'Ext.data.Model',
      fields:['id']
	});

    var sourceAjax = new Ext.data.proxy.Ajax({
      model: 'MxDevice',
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var sourceStore = Ext.create('Ext.data.Store',{
      model: 'MxDevice',
      autoLoad: false,
      pageSize: 10,
      proxy: sourceAjax
    });

    var fileAjax = new Ext.data.proxy.Ajax({
      model: 'MxFile',
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var fileStore = Ext.create('Ext.data.Store',{
      model: 'MxFile',
      autoLoad: false,
      pageSize: 10,
      proxy: fileAjax
    });

    var indexAjax = new Ext.data.proxy.Ajax({
      type: 'ajax',
      reader: {
        type: 'json', root: 'items', totalProperty: 'total'
      }
    });

    var indexStore = Ext.create('Ext.data.Store',{
      model: 'MxSourceID',
      autoLoad: false,
      pageSize: 10,
      proxy: indexAjax
    });

    var sourcePanel = Ext.create('Ext.grid.Panel',{
      store: sourceStore,
      title: '基本信息',
      columns: [{
        text:'编号',
        width:50,
        dataIndex:'id'
	  },{
		  text:'状态',
		width:75,
		dataIndex:'status'
	  },{
        text:'标题',
		width:100,
		dataIndex:'title'
	  },{
		  text:'正文简要信息',
		width:274,
		dataIndex:'description'
	  },{
		  text:'发表时间',
		  width:75,
		  dataIndex:'pushdate',
		  renderer: datetime_renderer
	  },{
		  text:'来源网址',
		  width:150,
		  dataIndex:'url',
	  },{
		  text:'是否包含图片',
		  width:75,
		  dataIndex:'ifimg'
	  },{
		  text:'语言',
		  width:100,
		  dataIndex:'lang'
	  },{
		  text:'修改时间',
		  width:75,
		  dataIndex:'updatetime',
		  renderer: datetime_renderer
	  },{
		  text:'作者',
		  width:50,
		  dataIndex:'creator'
	  }],
      listeners: {
        render: function(){}
      }
    });

    var appCombox = Ext.create('Ext.form.ComboBox', {
      fieldLabel: '应用',
      store: apps,
      editable: false,
      queryMode: 'local',
      displayField: 'label',
      valueField: 'app',
      listeners:{
        change: function(combobox,value){
          current_app = value;
          refresh();
        }
      }
    });

    var filelistPanel = Ext.create('Ext.grid.Panel',{
      store: fileStore,
      //width: WIDTH,
      height: HEIGHT-100,
      columns: [{
        text:'路径',
        width:200,
        dataIndex:'path'
      },{
        text:'修改时间',
        width:150,
        dataIndex:'mt',
        renderer:datetime_renderer
      },{
        text:'大小',
        width:100,
        dataIndex:'size',
        renderer:computersize_renderer
      }],
    });

    var filePanel = Ext.create('Ext.panel.Panel',{
      title: '文件',
      items: [appCombox,filelistPanel]
    });

    var indexPanel = Ext.create('Ext.grid.Panel',{
      store: indexStore,
      title: '抽取成功的 id 列表',
      columns: [{
        text:'id',
        width:300,
        dataIndex:'id',
        renderer:sourceid_renderer
      }],
      listeners: {
        render: function(){
        }
      }
    });

    var sourceText = Ext.create('Ext.form.field.Text', {
      width: 300,
      name: 'source',
      fieldLabel: 'source',
      regex: /^[a-zA-B1-9]+$/
    });

    var current_tab_title = sourcePanel.title;
    var current_app = false;

    function refresh(){
        var source = get_source();
        if(!source){
          return Ext.MessageBox.alert('警告','请输入正确的source！');
        }
        switch(current_tab_title){
        case sourcePanel.title:
          sourceAjax.url='job_service/v1/base?source='+source;
          sourceStore.removeAll(false);
          sourceStore.load();
          break;
        case filePanel.title:
          if(!current_app){
            return Ext.MessageBox.alert('警告','请选择应用！');
          }
          fileAjax.url = 'cloudservice/api/files?source='+source+'&app='+current_app;
          fileStore.removeAll(false);
          fileStore.load();
          break;
        case indexPanel.title:
          indexAjax.url = 'job_service/v1/ids?source=' + source;
          indexStore.removeAll(false);
          indexStore.load();
          break;
        }
    };

    var refreshButton = Ext.create('Ext.Button', {
      text: '刷新',
      handler: refresh
    });

    var tabPanel = Ext.create('Ext.tab.Panel',{
      height: HEIGHT,
      width: WIDTH,
      layout: 'card',
      items:[sourcePanel],
      listeners:{
        tabchange: function(panel,newcard,oldcard,opts){
          current_tab_title = newcard.title;
          if(current_tab_title!==filePanel.title){
            refresh();
          }
        }
      }
    });

    Ext.create('Ext.panel.Panel',{
      renderTo: Ext.getBody(),
      width: WIDTH,
      height: HEIGHT,
      title: 'job service monitor',
      layout: 'column',
      items: [
        sourceText,
        refreshButton,
        tabPanel
      ]
    });
  }
});

