function [Y, InitialMatrix] = parse_csv_file(file_path)
  % path -> a relative path to the .csv file
  
  % Y -> the vector with all actual values
  % InitialMatrix -> the matrix that must be transformed
  
  fid = fopen(file_path, 'r');
  header = fgetl(fid); % eliminare header tabel
  Y = InitialMatrix = [];
  
  format = ["%f",repmat("%s",1,12)]; % formatul pentru citirea coloanelor
  data = textscan(fid, format, 'delimiter',',');
  
  Y = data{1};
  for i = 2:13
    InitialMatrix = [InitialMatrix data{i}]; % adaugarea succesiva a coloanelor
  endfor                                      % data in matricea InitialMatrix
  
  fclose(fid);
endfunction