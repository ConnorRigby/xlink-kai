defmodule Xlink.Task do
  @moduledoc "FFS"

  def start_link(mod, fun, args) do
    Task.async(mod, fun, args) |> Task.await(:infinity)
    :ignore
  end
end
