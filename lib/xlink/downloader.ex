defmodule Xlink.Downloder do
  @moduledoc """
  Downloads and unpacks the xlink engine.
  """
  @url Application.get_env(:xlink, :dl_url)[:arm] || Mix.raise "no url"
  @dir Application.get_env(:xlink, :data_dir) || Mix.raise "no data dir"
  @dl_filename Application.get_env(:xlink, :filename) || Mix.raise "no filename"
  require Logger

  def dl_and_unarchive do
    if !File.exists?(@dir) do
      File.mkdir_p!(@dir)
    end

    file_contents = if !File.exists?(downloaded_file()) do
      download_file()
    else
      File.read!(downloaded_file())
    end

    case :erl_tar.extract({:binary, file_contents}, [:compressed, :memory]) do
      {:ok, file_list} ->
        file_contents = file_list |> Map.new() |> Map.get('kaiEngine-7.4.30/kaiengine')
        File.write(engine_file(), file_contents)
        File.chmod(engine_file(), 0o777)
      err -> exit("Error extracting archive: #{inspect err}")
    end
  end

  defp downloaded_file, do: Path.join(@dir, @dl_filename)
  defp engine_file, do: Path.join(@dir, "kaiengine")

  def download_file do
    Logger.info "Downloading #{@url} to #{downloaded_file()}"
    case :httpc.request(:get, {to_charlist(@url), []}, [], [body_format: :binary]) do
      {:ok, {{_, 200, _}, _, body}} ->
        :ok = File.write!(downloaded_file(), body)
        body
      err -> exit("Failed to download #{@url}: #{inspect err}")
    end
  end
end
