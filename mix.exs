defmodule Xlink.MixProject do
  use Mix.Project

  @target System.get_env("MIX_TARGET") || "host"

  Mix.shell().info([
    :green,
    """
    Mix environment
      MIX_TARGET:   #{@target}
      MIX_ENV:      #{Mix.env()}
    """,
    :reset
  ])

  def project do
    [
      app: :xlink,
      version: File.read!("VERSION") |> String.trim(),
      elixir: "~> 1.4",
      target: @target,
      archives: [nerves_bootstrap: "~> 1.0.0-rc"],
      deps_path: "deps/#{@target}",
      build_path: "_build/#{@target}",
      lockfile: "mix.lock.#{@target}",
      build_embedded: Mix.env() == :prod,
      start_permanent: Mix.env() == :prod,
      aliases: [loadconfig: [&bootstrap/1]],
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application, do: application(@target)

  # Specify target specific application configurations
  # It is common that the application start function will start and supervise
  # applications which could cause the host to fail. Because of this, we only
  # invoke Xlink.start/2 when running on a target.
  def application("host") do
    [extra_applications: [:logger, :inets, :ssl]]
  end

  def application(_target) do
    [mod: {Xlink.Application, []}, extra_applications: [:logger, :inets, :ssl]]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:nerves, "~> 1.0.0-rc", runtime: false},
      {:mdns, "~> 1.0"},
      {:poison, "~> 3.1"},
      {:ex_syslogger, "~> 1.4"}
    ] ++ deps(@target)
  end

  # Specify target specific dependencies
  defp deps("host"), do: []

  defp deps(target) do
    [
      {:shoehorn, "~> 0.2.0"},
      {:nerves_runtime, "~> 0.6.0"},
      {:nerves_network, "~> 0.3.7-rc0"},
      {:nerves_firmware_ssh, "~> 0.2"},
    ] ++ system(target)
  end

  defp system("rpi"), do: [{:nerves_system_rpi, "~> 1.0.0-rc", runtime: false}]
  defp system("rpi0"), do: [{:nerves_system_rpi0, "~> 1.0.0-rc", runtime: false}]
  defp system("rpi2"), do: [{:nerves_system_rpi2, "~> 1.0.0-rc", runtime: false}]
  defp system("rpi3"), do: [{:nerves_system_rpi3, "~> 1.0.0-rc", runtime: false}]
  defp system(target), do: Mix.raise("Unknown MIX_TARGET: #{target}")

  defp bootstrap(args) do
    Application.start(:nerves_bootstrap)
    Mix.Task.run("loadconfig", args)
  end
end
