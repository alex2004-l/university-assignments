function [Y, InitialMatrix] = parse_data_set_file(file_path)
  % path -> a relative path to the .txt file

  % Y -> the vector with all actual values
  % InitialMatrix -> the matrix that must be transformed
  fid = fopen(file_path, 'r');
  % citire dimensiuni matrice InitialMatrix
  m = fscanf(fid, "%d", 1);
  n = fscanf(fid, "%d", 1);
  
  for i = 1:m
    Y(i) = fscanf(fid, "%d",1); 
    for j = 1:n
      InitialMatrix{i,j} = fscanf(fid, "%s", 1); % toate elementele matricei se
    endfor                                       % citesc sub forma de stringuri
  endfor
  
  fclose(fid);
endfunction