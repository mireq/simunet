webConfigWidget.setPageTitle(document.title);

// spracujeme odkazy
aElements = document.getElementsByTagName('a');
for (i = 0; i < aElements.length; i++)
{
	element = aElements[i];
	if (element.hasAttribute('href'))
	{
		hrefAttr = element.getAttribute('href');
		element.setAttribute('href', 'javascript:webConfigWidget.setUrl("'+(hrefAttr.replace(/([\\"'])/g, "\\$1"))+'")');
	}
}

x = fa;