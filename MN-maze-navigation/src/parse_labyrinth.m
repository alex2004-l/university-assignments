function [Labyrinth] = parse_labyrinth(file_path)
	% file_path -> the relative path to a file that needs to
  %              be loaded_graphics_toolkits
  
  % Labyrinth -> the matrix of all encodings for the labyrinth's walls
  fid = fopen(file_path, 'r');
  
  % citire dimensiuni matrice
  m = fscanf(fid, "%d", 1);
  n = fscanf(fid, "%d", 1);
  
  % citire matrice
  Labyrinth = fscanf(fid, "%d", [m,n])';
  
  fclose(fid);

endfunction
