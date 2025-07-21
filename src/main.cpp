#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QVariantMap>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("CrystalBoard");
    QCoreApplication::setApplicationName("CrystalBoard");
    QCoreApplication::setApplicationVersion("1.1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription("A crystal-clear, transparent canvas that floats above your desktop.");
    parser.addHelpOption();
    parser.addVersionOption();

    // --- Behavior Control Options ---
    QCommandLineOption cleanOption({"c", "clean"}, "Start with a clean canvas, hiding the help panel.");
    parser.addOption(cleanOption);

    // --- Startup Configuration Options ---
    QCommandLineOption modeOption({"M", "mode"}, "Set the initial scroll mode.", "name", "History");
    parser.addOption(modeOption);

    QCommandLineOption hueOption({"H", "hue"}, "Set the initial color hue.", "0-359");
    parser.addOption(hueOption);

    QCommandLineOption saturationOption({"S", "saturation"}, "Set the initial color saturation.", "0-255");
    parser.addOption(saturationOption);

    QCommandLineOption valueOption({"V", "value"}, "Set the initial color value/brightness.", "0-255");
    parser.addOption(valueOption);

    QCommandLineOption opacityOption({"O", "opacity"}, "Set the initial color opacity.", "0-255");
    parser.addOption(opacityOption);

    QCommandLineOption sizeOption({"s", "size"}, "Set the initial brush/shape size.", "pixels");
    parser.addOption(sizeOption);

    QCommandLineOption textSizeOption({"t", "text-size"}, "Set the initial text size.", "pixels");
    parser.addOption(textSizeOption);

    QCommandLineOption toolOption({"T", "tool"}, "Set the initial tool.", "name", "Pen");
    parser.addOption(toolOption);

    // --- Other Options ---
    QCommandLineOption resetOption({"r", "reset"}, "Reset all saved settings to their defaults.");
    parser.addOption(resetOption);

    QCommandLineOption neverSaveOption({"n", "never-save"}, "Disable saving settings for this session.");
    parser.addOption(neverSaveOption);

    parser.process(a);

    // --- Package options for MainWindow ---
    QVariantMap cmdLineOptions;
    if (parser.isSet(cleanOption)) cmdLineOptions["clean"] = true;
    if (parser.isSet(resetOption)) cmdLineOptions["reset"] = true;
    if (parser.isSet(neverSaveOption)) cmdLineOptions["never-save"] = true;

    if (parser.isSet(modeOption)) cmdLineOptions["mode"] = parser.value(modeOption);
    if (parser.isSet(hueOption)) cmdLineOptions["hue"] = parser.value(hueOption).toInt();
    if (parser.isSet(saturationOption)) cmdLineOptions["saturation"] = parser.value(saturationOption).toInt();
    if (parser.isSet(valueOption)) cmdLineOptions["value"] = parser.value(valueOption).toInt();
    if (parser.isSet(opacityOption)) cmdLineOptions["opacity"] = parser.value(opacityOption).toInt();
    if (parser.isSet(sizeOption)) cmdLineOptions["size"] = parser.value(sizeOption).toInt();
    if (parser.isSet(textSizeOption)) cmdLineOptions["text-size"] = parser.value(textSizeOption).toInt();
    if (parser.isSet(toolOption)) cmdLineOptions["tool"] = parser.value(toolOption);

    MainWindow w(cmdLineOptions);
    w.show();

    return a.exec();
}