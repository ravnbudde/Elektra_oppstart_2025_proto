% startup.m – legg til alle undermapper rekursivt til path
projectRoot = fileparts(mfilename('fullpath'));
addpath(genpath(projectRoot));
disp(['[startup.m] Path lagt til: ' projectRoot]);
