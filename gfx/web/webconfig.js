webConfigWidget.setPageTitle(document.title);

function processMenu(element, depth)
{
	if (!element.hasChildNodes())
	{
		return;
	}

	for (i = 0; i < element.childNodes.length; i++)
	{
		child = element.childNodes[i];
		if (child.nodeName.toLowerCase() == 'li')
		{
			if (!child.hasChildNodes())
			{
				continue;
			}
			var itemMode = 0;
			if (child.hasAttribute('class'))
			{
				var className = child.getAttribute('class');
				if (className == 'expanded')
				{
					itemMode = 1;
				}
				else if (className = 'selected')
				{
					itemMode = 2;
				}
			}
			for (j = 0; j < child.childNodes.length; j++)
			{
				subnode = child.childNodes[j];
				if (subnode.nodeName.toLowerCase() == 'a')
				{
					if (!subnode.hasAttribute('href'))
					{
						continue;
					}
					if (!subnode.hasChildNodes())
					{
						continue;
					}
					var hrefAttr = subnode.getAttribute('href');
					var titleAttr = null;
					if (subnode.hasAttribute('title'))
					{
						titleAttr = subnode.getAttribute('title');
					}
					for (k = 0; k < subnode.childNodes.length; k++)
					{
						var textNode = subnode.childNodes[k];
						if (textNode.nodeType = textNode.TEXT_NODE)
						{
							webConfigWidget.addMenuItem(textNode.nodeValue, hrefAttr, depth, itemMode, titleAttr);
							break;
						}
					}
				}
				else if (subnode.nodeName.toLowerCase() == 'ul')
				{
					processMenu(subnode, depth + 1);
				}
			}
		}
	}
}

var menuElement = document.getElementById('menu');
if (menuElement != undefined)
{
	processMenu(menuElement, 0);
}

/*// spracujeme odkazy
aElements = document.getElementsByTagName('a');
for (i = 0; i < aElements.length; i++)
{
	element = aElements[i];
	if (element.hasAttribute('href'))
	{
		hrefAttr = element.getAttribute('href');
		element.setAttribute('href', 'javascript:webConfigWidget.setUrl("'+(hrefAttr.replace(/([\\"'])/g, "\\$1"))+'")');
	}
}*/

function switchMenu()
{
	menuElement = document.getElementById('menu');
	if (menuElement == undefined)
	{
		return;
	}
	if (menuElement.style.display == 'none')
	{
		showMenu();
	}
	else
	{
		hideMenu();
	}
}

//mainElementMargin = '0px';

function showMenu()
{
	menuElement = document.getElementById('menu');
	imgElement = document.getElementById('menu_hdr_img');
	mainElement = document.getElementById('main');
	if (menuElement == undefined || imgElement == undefined || mainElement == undefined)
	{
		return;
	}
	mainElement.className = 'main_menu_disp';
	menuElement.style.display = 'block';
	imgElement.src = 'qrc:/web/menu_h.png';
}

function hideMenu()
{
	menuElement = document.getElementById('menu');
	imgElement = document.getElementById('menu_hdr_img');
	mainElement = document.getElementById('main');
	if (menuElement == undefined || imgElement == undefined || mainElement == undefined)
	{
		return;
	}
	mainElement.className = 'main';
	menuElement.style.display = 'none';
	imgElement.src = 'qrc:/web/menu_v.png';
}

leftElement = document.getElementById('left');
if (leftElement != undefined)
{
	leftElement.innerHTML = '<div id="menu_hdr"><a href="javascript:switchMenu()"><img id="menu_hdr_img" src="qrc:/web/menu_v.png" /></a></div>'+leftElement.innerHTML;
}

hideMenu();

