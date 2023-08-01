# zarr-cpp
CPP interface to Zarr format

currently supports one var one chunk

## To build

```
cmake .
make 
```

## To execute 
`./cpp-zarr`

it generates test.zarr directory/dataset with one variable sst.

## to use analysis
`pip install --user -r requirements.txt` to install xarray and zarr.
`python open_zarr.py` 

