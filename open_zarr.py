import zarr
import xarray as xr

da=xr.open_zarr("test.zarr", consolidated=False)

print(da)

print(da.sst.values)
