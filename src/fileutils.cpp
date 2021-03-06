#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QStorageInfo>
#include <QTextStream>
#include <QDebug>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include "fileutils.h"

/*!
   Delete a directory along with all of its contents.

   param dirName Path of directory to remove.
   return true on success; false on error.
*/
bool FileUtils::removeDir(const QString &dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDir(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirName);
	}

	return result;
}


bool FileUtils::mkdir(const QString &dirName)
{
	bool result = QDir().mkdir(dirName);
	return result;
}


qint64 FileUtils::dirSize(const QString &dirPath) {
	qint64 size = 0;
	QDir dir(dirPath);
	//calculate total size of current directories' files
	QDir::Filters fileFilters = QDir::Files|QDir::System|QDir::Hidden;
	for(QString filePath : dir.entryList(fileFilters)) {
		QFileInfo fi(dir, filePath);
		size+= fi.size();
	}
	//add size of child directories recursively
	QDir::Filters dirFilters = QDir::Dirs|QDir::NoDotAndDotDot|QDir::System|QDir::Hidden;
	for(QString childDirPath : dir.entryList(dirFilters))
		size+= dirSize(dirPath + QDir::separator() + childDirPath);
	return size;
}


QString FileUtils::formatSize(const qint64 &size) {
	QStringList units = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
	int i;
	double outputSize = size;
	for(i=0; i<units.size()-1; i++) {
		if(outputSize<1024) break;
		outputSize= outputSize/1024;
	}
	return QString("%0 %1").arg(outputSize, 0, 'f', 2).arg(units[i]);
}


QString FileUtils::formattedDirSize(const QString &dirPath) {
	return FileUtils::formatSize(FileUtils::dirSize(dirPath));
}


qint64 FileUtils::totalStorage() {
	QStorageInfo storage = QStorageInfo::root();
	return storage.bytesTotal();
}


qint64 FileUtils::freeStorage() {
	QStorageInfo storage = QStorageInfo::root();
	return storage.bytesFree();
}


void FileUtils::touch(const QString path) {
	QFile* file = new QFile(path);
	file->open(QIODevice::ReadWrite);
	file->close();
}


void FileUtils::rm(const QString path) {
	QFile* file = new QFile(path);
	file->remove();
}


QString FileUtils::readFile(const QString path) {
	QFile* file = new QFile(path);
	if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream in(file);
		return in.readAll();
	} else {
		return "";
	}
};


void FileUtils::writeFile(const QString path, const QString txt) {
	QFile* file = new QFile(path);
	file->open(QIODevice::WriteOnly);
	file->write(txt.toStdString().c_str());
	file->close();
};


void FileUtils::mount(const QString configPath, const QString remotePath) {
	QString config = readFile(configPath);
	QString type = config.split("\n\n").at(0);
	if (type == "sshfs") {
		QStringList argv = config.split("\n\n").at(1).split("\n");
		QString user = argv.at(0);
		QString host = argv.at(1);
		QString port = argv.at(2);
		QString dir = argv.at(3);

		QString cmd = "sshfs -o allow_other " + user + "@" +
					  host + ":/" + dir + " " + remotePath + " -C -p " + port;

		exec(cmd.toStdString().c_str());
	} else {
		// Nothing
	}
};


QString FileUtils::whoami(void) {
	QString user = QString::fromStdString(exec("whoami"));
	user.chop(1);
	return user;
};


QString FileUtils::exec(const QString cmd) {
	std::string tmp = cmd.toStdString();
	return QString::fromStdString(exec(tmp.c_str()));
}


std::string FileUtils::exec(const char* cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}




