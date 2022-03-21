The point of this example is to show how to get a GDI font onto a 
DirectDrawSurface.   

This is a very simple example.  It doesn't do anything clever or speedy.   
It just creates a memory DC font and a DIB section, and uses them to get
access to the font bits once GDI has drawn them. The font bits are then 
shovelled to the primary surface.   They could easily be moved to an offscreen
surface which could then be bltted as needed, transparently or not (the 
FoxBear sample code does just this).
