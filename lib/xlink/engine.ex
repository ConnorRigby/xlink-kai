defmodule Xlink.Engine do
  @moduledoc "Handles monitoring the engine."
  require Logger
  @dir Application.get_env(:xlink, :data_dir) || Mix.raise("no data dir")
  @exe Path.join(@dir, "kaiengine")
  @doc false
  def start_link do
    GenServer.start_link(__MODULE__, [], name: __MODULE__)
  end

  def init([]) do
    opts = [
      :exit_status,
      :binary,
      :stderr_to_stdout,
      {:args, []}
    ]

    File.mkdir_p("/root/.xlink")

    unless File.exists?("/root/.xlink/kaiUserConfig.txt") do
      Logger.info("Coppying default configuration.")

      File.cp(
        Path.join(:code.priv_dir(:xlink), "kaiUserConfig.txt"),
        "/root/.xlink/kaiUserConfig.txt"
      )
    end

    port = Port.open({:spawn_executable, @exe}, opts)
    {:ok, %{port: port}}
  end

  def terminate(reason, state) do
    if match?({:engine_exit, _}, reason) do
      try_kill_port(state.port)
    else
      Logger.warn("Engine exit: #{inspect(reason)}")
    end
  end

  defp try_kill_port(nil), do: :ok

  defp try_kill_port(port) do
    case Port.info(port) do
      info when is_list(info) ->
        os_pid = Keyword.get(info, :os_pid)
        System.cmd("kill", ["-9", "#{os_pid}"])
        :ok

      _ ->
        :ok
    end
  end

  def handle_info({_, {:exit_status, 0}}, state) do
    Logger.info("Engine exited in a normal state.")
    {:stop, :normal, state}
  end

  def handle_info({_, {:exit_status, status}}, state) do
    Logger.info("Engine exited (#{status})")
    {:stop, {:engine_exit, status}, state}
  end

  def handle_info({_, {:data, data}}, state) do
    IO.puts(data)
    {:noreply, state}
  end
end
